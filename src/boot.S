#include "arm/sysregs.h"

.section ".text.boot"

// Make _start global.
.globl _start

    .org 0x80000
// Entry point for the kernel. Registers:
// x0 -> 32 bit pointer to DTB in memory (primary core only) / 0 (secondary cores)
// x1 -> 0
// x2 -> 0
// x3 -> 0
// x4 -> 32 bit kernel entry point, _start location
_start:
    mrs    x0, mpidr_el1
    and    x0, x0,#0xFF // Check processor id
    cbz    x0, master   // Hang for all non-primary CPU
    b    halt

master:
    // Read current exception level
    mrs x0, CurrentEL
    // Extract EL from CurrentEL
    lsr x0, x0, #2
    // Compare the exception level and branch accordingly
    cmp x0, #3
    b.eq el3_entry
    cmp x0, #2
    b.eq el2_entry
    cmp x0, #1
    b.eq el1_entry
    // Default to el1_entry
    b el1_entry

el2_entry: // if cpu is in el2 use this to set it to el1
    ldr    x0, =SCTLR_VALUE_MMU_DISABLED
    msr    sctlr_el1, x0

    ldr    x0, =HCR_VALUE
    msr    hcr_el2, x0

    ldr    x0, =SPSR_VALUE
    msr    spsr_el2, x0

    // Access to trace, SVE, Advanced SIMD and floating-point functionality.
    ldr    x0, =CPACR_TTA_FPEN_ZEN_ENABLE
    msr    cpacr_el1, x0

    adr    x0, el1_entry
    msr    elr_el2, x0

    eret

el3_entry: // if cpu is in el3 use this to set it to el3
    ldr    x0, =SCTLR_VALUE_MMU_DISABLED
    msr    sctlr_el1, x0

    ldr    x0, =HCR_VALUE
    msr    hcr_el2, x0

    ldr    x0, =SCR_VALUE
    msr    scr_el3, x0

    ldr    x0, =SPSR_VALUE
    msr    spsr_el3, x0

    // Access to trace, SVE, Advanced SIMD and floating-point functionality.
    ldr    x0, =CPACR_TTA_FPEN_ZEN_ENABLE
    msr    cpacr_el1, x0

    adr    x0, el1_entry
    msr    elr_el3, x0

    eret

el1_entry:
    // Set up stack pointer to _start
    ldr     x5, =_start
    mov     sp, x5

    // Clear BSS section
    ldr     x5, =__bss_start
    ldr     w6, =__bss_size
    1:  cbz     w6, 2f
        str     xzr, [x5], #8
        sub     w6, w6, #1
        cbnz    w6, 1b

    // Jump to C code, should not return
    2:  bl      kmain
    // For failsafe, halt this core
halt:
    wfe
    b halt

