#ifndef SYSREGS_H
#define SYSREGS_H

/* SCTLR_EL1, System Control Register (EL1), Page 2654 of
 * AArch64-Reference-Manual. */

#define SCTLR_RESERVED (3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
#define SCTLR_EE_LITTLE_ENDIAN (0 << 25)
#define SCTLR_EOE_LITTLE_ENDIAN (0 << 24)
#define SCTLR_I_CACHE_DISABLED (0 << 12)
#define SCTLR_D_CACHE_DISABLED (0 << 2)
#define SCTLR_MMU_DISABLED (0 << 0)
#define SCTLR_MMU_ENABLED (1 << 0)

#define SCTLR_VALUE_MMU_DISABLED \
    (SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED | SCTLR_D_CACHE_DISABLED | SCTLR_MMU_DISABLED)

/* HCR_EL2, Hypervisor Configuration Register (EL2), Page 2487 of
 * AArch64-Reference-Manual. */

#define HCR_RW (1 << 31)
#define HCR_VALUE HCR_RW

/* SCR_EL3, Secure Configuration Register (EL3), Page 2648 of
 * AArch64-Reference-Manual. */

#define SCR_RESERVED (3 << 4)
#define SCR_RW (1 << 10)
#define SCR_NS (1 << 0)
#define SCR_VALUE (SCR_RESERVED | SCR_RW | SCR_NS)

/* SPSR_EL3, Saved Program Status Register (EL3) Page 389 of
 * AArch64-Reference-Manual. */

#define SPSR_MASK_ALL (7 << 6)
#define SPSR_EL1h (5 << 0)
#define SPSR_VALUE (SPSR_MASK_ALL | SPSR_EL1h)

/* CPACR_EL1, Architectural Feature Access Control Register (EL1) Page 2411 of
 * AArch64-Reference-Manual. */

#define CPACR_TTA (1 << 28)
#define CPACR_FPEN(value) ((value & 0x3) << 20)
#define CPACR_ZEN(value) ((value & 0x3) << 16)

#define CPACR_TTA_FPEN_ZEN_ENABLE (CPACR_TTA | CPACR_FPEN(1) | CPACR_ZEN(1))

/* MAIR_EL1, Memory Attribute Indirection Register (EL1) Page 2609 of
 * AArch64-Reference-Manual. */

#define MT_DEVICE_nGnRnE 0x0
#define MT_NORMAL_NC 0x1
#define MT_READONLY 0x2
#define MT_DEVICE_nGnRnE_FLAGS 0x00
#define MT_NORMAL_NC_FLAGS 0x44
#define MT_READONLY_FLAGS 0xff
#define MAIR_VALUE (MT_DEVICE_nGnRnE_FLAGS << (8 * MT_DEVICE_nGnRnE)) | (MT_NORMAL_NC_FLAGS << (8 * MT_NORMAL_NC)) | (MT_READONLY_FLAGS) << (8 * MT_READONLY)

/* TCR_EL1, Translation Control Register (EL1) Page 2685 of
 * AArch64-Reference-Manual. */

#define TCR_T0SZ (64 - 36)
#define TCR_PS (0x01 << 16) // 36-bit physical address
#define TCR_TG0_4K (0 << 14)
#define TCR_IPS_64G (1ull << 32)
#define TCR_SH0_OUTER_SHAREABLE (0x2 << 12)
#define TCR_SH0_INNER_SHAREABLE (0x3 << 12)
#define OUTER_CACHEABLE (0x1 << 10)
#define INNER_CACHEABLE (0x1 << 8)
#define TCR_VALUE (TCR_IPS_64G | OUTER_CACHEABLE | INNER_CACHEABLE | TCR_T0SZ | TCR_PS | TCR_TG0_4K | TCR_SH0_INNER_SHAREABLE)

#endif /* SYSREGS_H */
