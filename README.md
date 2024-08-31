
clang -target aarch64-elf -c src/boot.S -o build/boot.o

clang -target aarch64-elf -ffreestanding -c src/main.c -o build/kernel.o -O2 -Wall -Wextra

ld.lld -T linker.ld -o os.elf -ffreestanding -O2 build/boot.o build/kernel.o

llvm-objcopy os.elf -O binary kernel8.img


ninja -t compdb > compile_commands.json
