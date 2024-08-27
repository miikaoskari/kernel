qemu-system-aarch64 -M raspi4b -serial stdio -serial pty -kernel os.elf -s -S &
lldb -o "target create os.elf" -o "gdb-remote 1234"
