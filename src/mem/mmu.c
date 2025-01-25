#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "mem/mem.h"
#include "mem/mmu.h"
#include "mmio/mmio.h"
#include "peripherals/bcm2711/cpu.h"

/* raspi4b address map
 * SDRAM 0x0_0000_0000 - 0x0_4000_0000
 * Peripherals 0x0_FC00_0000 - 0x1_0000_0000
 * PCIe 0x6_0000_0000 - 0x7_FFFF_FFFF
 */

/* use granule of 4KB.
 * use table indexes from 0 to 3.
 *
 * virtual address bits:
 * 47:39 level 0 table index.
 * 38:30 level 1 table index.
 * 29:21 level 2 table index.
 * 20:12 level 3 table index.
 * 11:0 block offset.
 *
 * each entry in pl0 can point to L1 table.
 * no block entries!
 * pl0 is split as higher half kernel
 *
 * +-------------------+--------------------+----------+---------------------------+
 * | Start             | End                | Size     | Use                       |
 * +-------------------+--------------------+----------+---------------------------+
 * | 0000000000000000  | 0000ffffffffffff   | 256TB    | user                      |
 * | ffff000000000000  | ffff7fffffffffff   | 128TB    | kernel logical memory map |
 * +-------------------+--------------------+----------+---------------------------+
 *
 * translation table lookup with 4KB pages
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 * |63    56|55    48|47    40|39    32|31    24|23    16|15     8|7      0|
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 *  |                 |         |         |         |         |
 *  |                 |         |         |         |         v
 *  |                 |         |         |         |   [11:0]  in-page offset
 *  |                 |         |         |         +-> [20:12] L3 index
 *  |                 |         |         +-----------> [29:21] L2 index
 *  |                 |         +---------------------> [38:30] L1 index
 *  |                 +-------------------------------> [47:39] L0 index
 *  +-------------------------------------------------> [63] TTBR0/1*
 * from linux/Documentation/arch/arm64/memory.rst
 */

#ifdef __aarch64__
#define BCM_VERSION 2711
// Each entry is a gig.
uint64_t level_1_table[512] __attribute__((aligned(4096)));

// First gig is stack, code, rodata then data, bss, ram and then GPU.
uint64_t level_2_0x0_0000_0000_to_0x0_4000_0000[512] __attribute__((aligned(4096)));

#if BCM_VERSION == 2711
// Third gig has peripherals
uint64_t level_2_0x0_c000_0000_to_0x1_0000_0000[512] __attribute__((aligned(4096)));
#endif
#else
// Each entry is a megabyte
uint32_t level_1_table[1024] __attribute__((aligned(4096)));
#endif

STRICT_ALIGN void setup_mmu_flat_map(void) {
    // Each entry is 2MB or 0x20_0000
    // First two MB
    level_2_0x0_0000_0000_to_0x0_4000_0000[0] = 0x0000000000000000 |
                                                MM_DESCRIPTOR_MAIR_INDEX(MT_READONLY) |
                                                MM_DESCRIPTOR_INNER_SHAREABLE |
                                                MM_DESCRIPTOR_ACCESS_FLAG |
                                                MM_DESCRIPTOR_BLOCK |
                                                MM_DESCRIPTOR_VALID;
    for (uint64_t i = 1; i < 512 - 8; i++) {
        level_2_0x0_0000_0000_to_0x0_4000_0000[i] = (0x0000000000000000 + (i << 21)) |
                                                    MM_DESCRIPTOR_EXECUTE_NEVER |
                                                    MM_DESCRIPTOR_MAIR_INDEX(MT_READONLY) |
                                                    MM_DESCRIPTOR_INNER_SHAREABLE |
                                                    MM_DESCRIPTOR_ACCESS_FLAG |
                                                    MM_DESCRIPTOR_BLOCK |
                                                    MM_DESCRIPTOR_VALID;
    }
    // Last 16 MB are shared with the GPU.
    for (uint64_t i = 512 - 8; i < 512; i++) {
        level_2_0x0_0000_0000_to_0x0_4000_0000[i] = (0x0000000000000000 + (i << 21)) |
                                                    MM_DESCRIPTOR_EXECUTE_NEVER |
                                                    MM_DESCRIPTOR_MAIR_INDEX(MT_READONLY) |
                                                    MM_DESCRIPTOR_INNER_SHAREABLE |
                                                    MM_DESCRIPTOR_ACCESS_FLAG |
                                                    MM_DESCRIPTOR_BLOCK |
                                                    MM_DESCRIPTOR_VALID;
    }
    level_1_table[0] = ((uint64_t) level_2_0x0_0000_0000_to_0x0_4000_0000) |
                       MM_DESCRIPTOR_TABLE |
                       MM_DESCRIPTOR_VALID;

    // Set peripherals to register access.
    for (uint64_t i = 480; i < 512; i++) {
        level_2_0x0_c000_0000_to_0x1_0000_0000[i] = (0x00000000c0000000 + (i << 21)) |
                                                    MM_DESCRIPTOR_EXECUTE_NEVER |
                                                    MM_DESCRIPTOR_MAIR_INDEX(MT_DEVICE_nGnRnE) |
                                                    MM_DESCRIPTOR_ACCESS_FLAG |
                                                    MM_DESCRIPTOR_BLOCK |
                                                    MM_DESCRIPTOR_VALID;
    }
    level_1_table[3] = ((uint64_t) level_2_0x0_c000_0000_to_0x1_0000_0000) |
                       MM_DESCRIPTOR_TABLE |
                       MM_DESCRIPTOR_VALID;

    uint64_t mair = MAIR_VALUE;
    uint64_t tcr = TCR_VALUE;
    uint64_t ttbr0 = ((uint64_t) level_1_table) | MM_TTBR_CNP;
    uint64_t sctlr = 0;
    asm volatile (
        // The ISB forces these changes to be seen before any other registers are changed
        "ISB\n\t"
        // Clear the TLB
        "TLBI VMALLE1\n\t"
        // Set MAIR
        "MSR MAIR_EL1, %[mair]\n\t"
        // Set TTBR0
        "MSR TTBR0_EL1, %[ttbr0]\n\t"
        // Set TCR
        "MSR TCR_EL1, %[tcr]\n\t"
        // The ISB forces these changes to be seen before the MMU is enabled.
        "ISB\n\t"
        // Read System Control Register configuration data
        "MRS %[sctlr], SCTLR_EL1\n\t"
        // Set [M] bit and enable the MMU.
        "ORR %[sctlr], %[sctlr], #0x1\n\t"
        "dsb ish\n\t"
        "isb\n\t"
        "MSR SCTLR_EL1, %[sctlr]\n\t"
        // The ISB forces these changes to be seen by the next instruction
        "ISB\n\t"
        : /* No outputs. */
        : [mair] "r" (mair),
          [tcr] "r" (tcr),
          [ttbr0] "r" (ttbr0),
          [sctlr] "r" (sctlr)
    );
}
