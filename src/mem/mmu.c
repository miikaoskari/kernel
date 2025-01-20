#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "mem/mem.h"
#include "mem/mmu.h"
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

uint64_t l0_table[TABLE_SIZE] __attribute__((aligned(4096)));
uint64_t l1_table[TABLE_SIZE] __attribute__((aligned(4096)));
uint64_t l2_table[TABLE_SIZE] __attribute__((aligned(4096)));
uint64_t l3_table[TABLE_SIZE] __attribute__((aligned(4096)));

static void paging_init(void)
{
    l0_table[0] = (uint64_t)l1_table
        | MM_DESCRIPTOR_TABLE
        | MM_DESCRIPTOR_VALID;

    l1_table[0] = (uint64_t)l2_table
        | MM_DESCRIPTOR_TABLE
        | MM_DESCRIPTOR_VALID;

    l2_table[0] = (uint64_t)l3_table
        | MM_DESCRIPTOR_TABLE
        | MM_DESCRIPTOR_VALID;

    /* map sdram */
    for (uint64_t i = 0; i < 0x40000; i++)
    {
        uint64_t paddr = i * PAGE_SIZE;
        l3_table[i] = paddr
            | MMU_FLAGS
            | MM_DESCRIPTOR_VALID;
    }

    /* map peripherals */
    for (uint64_t i = 0xFC00; i < 0x10000; i++)
    {
        uint64_t paddr = i * PAGE_SIZE;
        l3_table[i] = paddr
            | MMU_DEVICE_FLAGS
            | MM_DESCRIPTOR_VALID;
    }
}

static void enable_mmu(void)
{
    /* configure the sysregs to use mmu */
    uint64_t mair = MAIR_VALUE;
    uint64_t tcr = TCR_VALUE;

    /* user */
    uint64_t ttbr0 = ((uint64_t)l0_table) | MM_TTBR_CNP;
    /* kernel */
    uint64_t ttbr1 = ((uint64_t)l0_table) | MM_TTBR_CNP;

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
        // Set TTBR1
        "MSR TTBR1_EL1, %[ttbr1]\n\t"
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
          [ttbr1] "r" (ttbr1),
          [sctlr] "r" (sctlr)
    );
}

STRICT_ALIGN void mmu_init(void)
{
    /* init 4 levels of pages. */
    paging_init();

    /* enable mmu */
    enable_mmu();
}
