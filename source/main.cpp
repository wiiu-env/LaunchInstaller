#include <coreinit/cache.h>
#include <coreinit/debug.h>
#include <coreinit/ios.h>
#include <coreinit/title.h>
#include <dirent.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sysapp/launch.h>
#include <sysapp/title.h>

#include <romfs_dev.h>

typedef struct __attribute((packed)) {
    uint32_t command;
    uint32_t target;
    uint32_t filesize;
    uint32_t fileoffset;
    char path[256];
} LOAD_REQUEST;

extern "C" void _SYSLaunchTitleWithStdArgsInNoSplash(uint64_t, int);
extern "C" void OSRestartGame();

bool StringEndsWith(const std::string &a, const std::string &b) {
    if (b.size() > a.size())
        return false;
    return std::equal(a.begin() + a.size() - b.size(), a.end(), b.begin());
}

int32_t getRPXInfoForPath(const std::string &path, romfs_fileInfo *info) {
    if (romfsMount("rcc", path.c_str(), RomfsSource_FileDescriptor_CafeOS) < 0) {
        return -1;
    }
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir("rcc:/code/"))) {
        romfsUnmount("rcc");
        return -2;
    }
    bool found = false;
    int res    = -3;
    while ((entry = readdir(dir)) != nullptr) {
        if (StringEndsWith(entry->d_name, ".rpx")) {
            if (romfsGetFileInfoPerPath("rcc", (std::string("code/") + entry->d_name).c_str(), info) >= 0) {
                found = true;
                res   = 0;
            }
            break;
        }
    }

    closedir(dir);

    romfsUnmount("rcc");

    if (!found) {
        return -4;
    }
    return res;
}

int main(int argc, char **argv) {
    std::string bundle_path  = "wiiu/apps/PayloadLoaderInstaller.wuhb";
    std::string completePath = std::string("/vol/external01/") + bundle_path;

    if (fopen(completePath.c_str(), "r") == NULL) {
        OSFatal("\"sd:/wiiu/apps/PayloadLoaderInstaller.wuhb\" is missing on the sd card");
        ;
    }

    LOAD_REQUEST request;
    memset(&request, 0, sizeof(request));

    request.command    = 0xFC; // IPC_CUSTOM_LOAD_CUSTOM_RPX;
    request.target     = 0;    // LOAD_FILE_TARGET_SD_CARD
    request.filesize   = 0;    // unknown filesize
    request.fileoffset = 0;    //
    request.path[0]    = '\0';


    romfs_fileInfo info;
    int res = getRPXInfoForPath(completePath, &info);

    if (res >= 0) {
        request.filesize   = ((uint32_t *) &info.length)[1];
        request.fileoffset = ((uint32_t *) &info.offset)[1];
    }

    strncat(request.path, bundle_path.c_str(), sizeof(request.path) - 1);
    DCFlushRange(&request, sizeof(LOAD_REQUEST));

    int mcpFd = IOS_Open("/dev/mcp", (IOSOpenMode) 0);
    if (mcpFd >= 0) {
        int out = 0;
        IOS_Ioctl(mcpFd, 100, &request, sizeof(request), &out, sizeof(out));
        IOS_Close(mcpFd);
    }

    uint64_t titleID = _SYSGetSystemApplicationTitleId(SYSTEM_APP_ID_HEALTH_AND_SAFETY);
    if (OSGetTitleID() == titleID) {
        OSRestartGame();
    } else {
        _SYSLaunchTitleWithStdArgsInNoSplash(titleID, 0);
    }

    return 0;
}
