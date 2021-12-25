#include <string.h>
#include <coreinit/title.h>
#include <sysapp/title.h>
#include <sysapp/launch.h>
#include <coreinit/ios.h>
#include <coreinit/cache.h>

typedef struct __attribute((packed)) {
    uint32_t command;
    uint32_t target;
    uint32_t filesize;
    uint32_t fileoffset;
    char path[256];
} LOAD_REQUEST;

extern "C" void _SYSLaunchTitleWithStdArgsInNoSplash(uint64_t, int);
extern "C" void OSRestartGame();

int main(int argc, char **argv) {
    LOAD_REQUEST request;
    memset(&request, 0, sizeof(request));

    request.command = 0xFC; // IPC_CUSTOM_LOAD_CUSTOM_RPX;
    request.target = 0;     // LOAD_FILE_TARGET_SD_CARD
    request.filesize = 0;   // unknown filesize
    request.fileoffset = 0; //    
    request.path[0] = '\0';

    strncat(request.path, "/wiiu/apps/payload_loader_installer.rpx", sizeof(request.path) - 1);
    DCFlushRange(&request, sizeof(LOAD_REQUEST));

    int mcpFd = IOS_Open("/dev/mcp", (IOSOpenMode) 0);
    if (mcpFd >= 0) {
        int out = 0;
        IOS_Ioctl(mcpFd, 100, &request, sizeof(request), &out, sizeof(out));
        
        int in = 0xFA; // IPC_CUSTOM_START_USB_LOGGING       
        IOS_Ioctl(mcpFd, 100, &in, sizeof(in), &out, sizeof(out));

        IOS_Close(mcpFd);
    }

    uint64_t titleID = _SYSGetSystemApplicationTitleId(SYSTEM_APP_ID_HEALTH_AND_SAFETY);
    if( OSGetTitleID() == titleID){
        OSRestartGame();
    } else {
        _SYSLaunchTitleWithStdArgsInNoSplash(titleID, 0);
    }
    
    return 0;
}
