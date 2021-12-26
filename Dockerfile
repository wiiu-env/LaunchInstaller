FROM wiiuenv/devkitppc:20211106

COPY --from=wiiuenv/libromfs_wiiu:20210924 /artifacts $DEVKITPRO

WORKDIR project