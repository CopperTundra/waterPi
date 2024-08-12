set sysroot /var/lib/schroot/chroots/rpi3-bookworm-arm64
target remote | ssh RPi3 gdbserver - '~/bin/waterPi -c config.json'
