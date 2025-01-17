#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "mem/mmu.h"
#include "peripherals/bcm2711/cpu.h"

/* Each entry is 1GB total 512GB */
uint64_t l1_table[512] __attribute__((aligned(4096)));
uint64_t l2_user_table[512] __attribute__((aligned(4096)));
uint64_t l2_kernel_table[512] __attribute__((aligned(4096)));



STRICT_ALIGN void mmu_init(void)
{
    uint64_t mair = MAIR_VALUE;
    uint64_t tcr = TCR_VALUE;
    uint64_t ttbr0 = ((uint64_t)l1_table) | MM_TTBR_CNP;
    uint64_t ttbr1 = ((uint64_t)l1_table) | MM_TTBR_CNP;

    uint64_t sctlr = 0;
    asm volatile(
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
        : [mair] "r"(mair),
        [tcr] "r"(tcr),
        [ttbr0] "r"(ttbr0),
        [ttbr1] "r"(ttbr1),
        [sctlr] "r"(sctlr));
}
