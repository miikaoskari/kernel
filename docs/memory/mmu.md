# Memory Structure

- By default the Raspberry Pi 4b is in low peripheral mode.

- 64 bit address size 0xFFFF_FFFF_FFFF_FFFF
![image](assets/memory_map.png)

- The memory design will be implemented as [Higher Half Kernel](https://wiki.osdev.org/Higher_Half_Kernel).

- The ASID is stored in one of the two TTBRn_EL1 registers. TTBR0_EL1 is used for user space and TTBR1_EL1 is used for kernel space.

- Will implement multilevel translation
  - Maximum number of tables is 6 for Armv8-A.
  - Levels go from -2 to 3.
  - Kernel will implement 3 levels

- Granule size will be 4KB
