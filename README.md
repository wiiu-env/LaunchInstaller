# LaunchInstaller

## Usage
Place the `90_launch_installer.rpx` in the `[ENVIRONMENT]/modules/setup` folder and run the [EnvironmentLoader](https://github.com/wiiu-env/EnvironmentLoader).
Requires [MochaPayload](https://github.com/wiiu-env/MochaPayload) as a setup module in `[ENVIRONMENT]/modules/setup`.
Will automatically launch `sd:/wiiu/apps/PayloadLoaderInstaller.wuhb`

## Building

For building you just need [wut](https://github.com/devkitPro/wut/) installed, then use the `make` command.

## Building using the Dockerfile

It's possible to use a docker image for building. This way you don't need anything installed on your host system.

```
# Build docker image (only needed once)
docker build . -t launchinstaller-builder

# make 
docker run -it --rm -v ${PWD}:/project launchinstaller-builder make

# make clean
docker run -it --rm -v ${PWD}:/project launchinstaller-builder make clean
```

## Credits
Maschell