# kernel

## building

generate the build.ninja file with
```bash
python conf.py
```

then build with `ninja`

## launch

### inside qemu
use `run.sh` to launch the kernel in qemu

### with raspi4b
setup the raspi4b with jtag connection.

install openocd.

`sudo pacman -S openocd`

launch openocd with `openocd -f interface/jlink.cfg -f board/rpi4b.cfg`

connect with `aarch64-none-elf-gdb kernel8.elf`

set the target to the bcm2711.cpu0 `(gdb) target extended-remote :3333`

load the kernel8.elf to the raspi4b `(gdb) load kernel8.elf`

start debugging!

## info

- raspi4b is in low peripheral mode by default!
