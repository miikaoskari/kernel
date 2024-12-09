#ifndef P_ARMC_IRQ_H
#define P_ARMC_IRQ_H

#define ARMC_BASE		(0x7e00b000)

/* IRQ Status */
#define IRQ_STATUS0		(ARMC_BASE + 0x230)
#define IRQ_STATUS1		(ARMC_BASE + 0x234)
#define IRQ_STATUS2		(ARMC_BASE + 0x238)

/* ARM Core 0 IRQs */
#define IRQ0_PENDING0		(ARMC_BASE + 0x200)
#define IRQ0_PENDING1		(ARMC_BASE + 0x204)
#define IRQ0_PENDING2		(ARMC_BASE + 0x208)
#define IRQ0_SET_EN_0		(ARMC_BASE + 0x210)
#define IRQ0_SET_EN_1		(ARMC_BASE + 0x214)
#define IRQ0_SET_EN_2		(ARMC_BASE + 0x218)
#define IRQ0_CLR_EN_0		(ARMC_BASE + 0x220)
#define IRQ0_CLR_EN_1		(ARMC_BASE + 0x224)
#define IRQ0_CLR_EN_2		(ARMC_BASE + 0x228)

/* ARM Core 0 FIQs 
 * TODO: Correct the values */
#define FIQ0_PENDING0		(ARMC_BASE + 0x300)
#define FIQ0_PENDING1		(ARMC_BASE + 0x304)
#define FIQ0_PENDING2		(ARMC_BASE + 0x308)
#define FIQ0_SET_EN_0		(ARMC_BASE + 0x308)
#define FIQ0_SET_EN_1		(ARMC_BASE + 0x308)
#define FIQ0_SET_EN_2		(ARMC_BASE + 0x308)
#define FIQ0_CLR_EN_0		(ARMC_BASE + 0x308)
#define FIQ0_CLR_EN_1		(ARMC_BASE + 0x308)
#define FIQ0_CLR_EN_2		(ARMC_BASE + 0x308)

/* ARM Core 1 IRQs */
#define IRQ1_PENDING0		(ARMC_BASE + 0x240)
#define IRQ1_PENDING1		(ARMC_BASE + 0x244)
#define IRQ1_PENDING2		(ARMC_BASE + 0x248)
#define IRQ1_SET_EN_0		(ARMC_BASE + 0x250)
#define IRQ1_SET_EN_1		(ARMC_BASE + 0x254)
#define IRQ1_SET_EN_2		(ARMC_BASE + 0x258)
#define IRQ1_CLR_EN_0		(ARMC_BASE + 0x260)
#define IRQ1_CLR_EN_1		(ARMC_BASE + 0x264)
#define IRQ1_CLR_EN_2		(ARMC_BASE + 0x268)

/* ARM Core 1 FIQs 
 * TODO: Correct the values */
#define FIQ1_PENDING0		(ARMC_BASE + 0x300)
#define FIQ1_PENDING1		(ARMC_BASE + 0x304)
#define FIQ1_PENDING2		(ARMC_BASE + 0x308)
#define FIQ1_SET_EN_0		(ARMC_BASE + 0x308)
#define FIQ1_SET_EN_1		(ARMC_BASE + 0x308)
#define FIQ1_SET_EN_2		(ARMC_BASE + 0x308)
#define FIQ1_CLR_EN_0		(ARMC_BASE + 0x308)
#define FIQ1_CLR_EN_1		(ARMC_BASE + 0x308)
#define FIQ1_CLR_EN_2		(ARMC_BASE + 0x308)

/* ARM Core 2 IRQs */
#define IRQ2_PENDING0		(ARMC_BASE + 0x280)
#define IRQ2_PENDING1		(ARMC_BASE + 0x284)
#define IRQ2_PENDING2		(ARMC_BASE + 0x288)
#define IRQ2_SET_EN_0		(ARMC_BASE + 0x290)
#define IRQ2_SET_EN_1		(ARMC_BASE + 0x294)
#define IRQ2_SET_EN_2		(ARMC_BASE + 0x298)
#define IRQ2_CLR_EN_0		(ARMC_BASE + 0x2a0)
#define IRQ2_CLR_EN_1		(ARMC_BASE + 0x2a4)
#define IRQ2_CLR_EN_2		(ARMC_BASE + 0x2a8)

/* ARM Core 2 FIQs 
 * TODO: Correct the values */
#define FIQ2_PENDING0		(ARMC_BASE + 0x300)
#define FIQ2_PENDING1		(ARMC_BASE + 0x304)
#define FIQ2_PENDING2		(ARMC_BASE + 0x308)
#define FIQ2_SET_EN_0		(ARMC_BASE + 0x308)
#define FIQ2_SET_EN_1		(ARMC_BASE + 0x308)
#define FIQ2_SET_EN_2		(ARMC_BASE + 0x308)
#define FIQ2_CLR_EN_0		(ARMC_BASE + 0x308)
#define FIQ2_CLR_EN_1		(ARMC_BASE + 0x308)
#define FIQ2_CLR_EN_2		(ARMC_BASE + 0x308)

/* ARM Core 3 IRQs */
#define IRQ3_PENDING0		(ARMC_BASE + 0x2c0)
#define IRQ3_PENDING1		(ARMC_BASE + 0x2c4)
#define IRQ3_PENDING2		(ARMC_BASE + 0x2c8)
#define IRQ3_SET_EN_0		(ARMC_BASE + 0x2d0)
#define IRQ3_SET_EN_1		(ARMC_BASE + 0x2d4)
#define IRQ3_SET_EN_2		(ARMC_BASE + 0x2d8)
#define IRQ3_CLR_EN_0		(ARMC_BASE + 0x2e0)
#define IRQ3_CLR_EN_1		(ARMC_BASE + 0x2e4)
#define IRQ3_CLR_EN_2		(ARMC_BASE + 0x2e8)

/* ARM Core 3 FIQs 
 * TODO: Correct the values */
#define FIQ3_PENDING0		(ARMC_BASE + 0x300)
#define FIQ3_PENDING1		(ARMC_BASE + 0x304)
#define FIQ3_PENDING2		(ARMC_BASE + 0x308)
#define FIQ3_SET_EN_0		(ARMC_BASE + 0x308)
#define FIQ3_SET_EN_1		(ARMC_BASE + 0x308)
#define FIQ3_SET_EN_2		(ARMC_BASE + 0x308)
#define FIQ3_CLR_EN_0		(ARMC_BASE + 0x308)
#define FIQ3_CLR_EN_1		(ARMC_BASE + 0x308)
#define FIQ3_CLR_EN_2		(ARMC_BASE + 0x308)

/* SWIRQ */
#define SWIRQ_SET		(ARMC_BASE + 0x3f0)
#define SWIRQ_CLEAR		(ARMC_BASE + 0x3f4)

#endif /* P_ARMC_IRQ_H */
