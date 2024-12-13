qemu-system-aarch64 -M raspi4b -serial stdio -serial pty -kernel kernel8.elf -s -S &
lldb -o "target create kernel8.elf" -o "gdb-remote 1234"
