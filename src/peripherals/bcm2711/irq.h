#ifndef BCM2711_IRQ_H
#define BCM2711_IRQ_H

/* GIC-400 (GICV4) Registers
 * Page 33 CoreLink GIC-400 Generic Interrupt Controller
 * Technical Reference Manual
 * Revision r0p1 */
#define GIC_BASE		(0x4c0040000)
#define GICD_BASE		(GIC_BASE + 0x1000)
#define GIDD_BASE		(GIC_BASE + 0x2000)
#define GICH_BASE		(GIC_BASE + 0x4000)
#define GICV_BASE		(GIC_BASE + 0x5000)

/* GIC Distributor Registers
 * Page 34-35 CoreLink GIC-400 Generic Interrupt Controller */
#define GICD_ISENABLER_BASE	(GICD_BASE + 0x0100)
#define GICD_ITARGETSR_BASE	(GICD_BASE + 0x800)

/* GIC CPU Interface Registers
 * Page 40 CoreLink GIC-400 Generic Interrupt Controller */
#define GICC_IAR		(GICC_BASE + 0x000c)
#define GICC_EOIR		(GICC_BASE + 0x0010)

/* VideoCore (VC) Interrupts
 * starts from Interrupt ID dec 96 */
#define SYSTEM_TIMER_IRQ_0	(0x60)
#define SYSTEM_TIMER_IRQ_1	(0x61)
#define SYSTEM_TIMER_IRQ_2	(0x62)
#define SYSTEM_TIMER_IRQ_3	(0x63)

#endif /* BCM2711_IRQ_H */
