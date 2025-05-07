add_rules("mode.debug", "mode.release")

toolchain("aarch64-none-elf")
    set_toolset("cc", "aarch64-none-elf-gcc")
    set_toolset("as", "aarch64-none-elf-gcc")
    set_toolset("ld", "aarch64-none-elf-ld")
toolchain_end()

-- set the toolchain and language
set_toolchains("aarch64-none-elf")
set_arch("aarch64")
set_languages("gnu17")

-- define the kernel target
target("kernel8.elf")
    set_kind("binary")

    add_files("src/**/*.c",
    "src/**/*.S",
    "arch/aarch64/**/*.c",
    "arch/aarch64/**/*.S",
    "arch/aarch64/*.S",
    "arch/aarch64/*.c")
    add_files("external/printk/*.c")
    add_files("linker8.ld")

    add_includedirs("src",
    "arch/aarch64",
    "external/printk")

    add_cflags("-ffreestanding", {force = true})
    add_cflags("-Wall", "-Wextra")

target("u-boot")
    set_kind("phony")
    on_build(function (target)
        if not os.isfile("external/u-boot/u-boot.bin") then
            os.exec("make -j$(nproc) -C external/u-boot qemu_arm64_defconfig")
            os.exec("make -j$(nproc) -C external/u-boot CROSS_COMPILE=aarch64-none-elf-")
        end
    end)
