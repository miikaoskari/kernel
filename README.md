# mios kernel

## Building

generate the build.ninja file with
`python conf.py`

then build with `ninja`

## Launch

### Raspberry Pi 4b
Setup the raspi4b with jtag connection.

Install OpenOCD.

`$ sudo pacman -S openocd`

Launch OpenOCD with `$ openocd -f interface/jlink.cfg -f board/rpi4b.cfg`

Connect with `$ aarch64-none-elf-gdb kernel8.elf`

Set the target to the bcm2711.cpu0 `(gdb) target extended-remote :3333`

Load the kernel8.elf to the raspi4b `(gdb) load kernel8.elf`

Happy debugging!

## Info

- Raspi4b is in low peripheral mode by default!
