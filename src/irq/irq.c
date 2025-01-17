#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "irq/irq.h"
#include "peripherals/bcm2711/bcm2711_lpa.h"
#include "peripherals/bcm2711/core_ca72.h"
#include "peripherals/bcm2711/cpu.h"
#include "peripherals/bcm2711/interrupt_handlers.h"
#include "utils/printk/printk.h"

const char* entry_error_messages[] = {
    "SYNC_INVALID_EL1t",
    "IRQ_INVALID_EL1t",
    "FIQ_INVALID_EL1t",
    "ERROR_INVALID_EL1t",

    "SYNC_INVALID_EL1h",
    "IRQ_INVALID_EL1h",
    "FIQ_INVALID_EL1h",
    "ERROR_INVALID_EL1h",

    "SYNC_INVALID_EL0_64",
    "IRQ_INVALID_EL0_64",
    "FIQ_INVALID_EL0_64",
    "ERROR_INVALID_EL0_64",

    "SYNC_INVALID_EL0_32",
    "IRQ_INVALID_EL0_32",
    "FIQ_INVALID_EL0_32",
    "ERROR_INVALID_EL0_32",
};

/* get current cpu using mpidr register */
static uint8_t get_current_cpu(void)
{
    uint32_t mpidr = 0;
    asm volatile("mrs     %[mpidr], mpidr_el1"
        : /* No outputs. */
        : [mpidr] "r"(mpidr));
    return mpidr & 0xff;
}

/* enable cpu local irqs */
void enable_irqs(void)
{
    /* [0..0] Enable signaling of group 0 */
    GIC_CPU->GICC_CTLR_b.ENABLE_GROUP_0 = true;

    /* enable irqs */
    asm volatile("msr   daifclr, #2");
    asm volatile("isb");
    return;
}

/* disable irqs */
void disable_irqs(void)
{
    /* disable IRQ with daifset */
    asm volatile("msr   daifset, #2");
    asm volatile("isb");
    return;
}

/* enable specific irq type */
void enable_irq(IRQn_Type irq)
{
    COMPLETE_MEMORY_READS;
    volatile uint8_t* targets = (volatile uint8_t*)&GIC_DIST->GICD_ITARGETSR;
    targets[irq] |= 1 << get_current_cpu();
    volatile uint32_t* enabled = (volatile uint32_t*)&GIC_DIST->GICD_ISENABLER;
    enabled[irq / 32] = 1 << (irq % 32);
    COMPLETE_MEMORY_READS;
    return;
}

void enable_interrupt_controller(void)
{
    /* enable system timer irq */
    enable_irq(TIMER_0_IRQn);
}

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address)
{
    printk("%s, ESR: %x, address: %x\r\n", entry_error_messages[type], esr, address);
}

void handle_irq(void)
{
    COMPLETE_MEMORY_READS;

    while (GIC_CPU->GICC_HPPIR_b.INTERRUPT_ID < INTERRUPT_COUNT) {
        /* Get the interrupt acknowledge register.
         * Register changes state after reading. */
        uint32_t current_interrupt = GIC_CPU->GICC_IAR;

        /* Get id of the current interrupt */
        uint32_t interrupt_id = current_interrupt & ARM_GIC400_CPU_GICC_IAR_INTERRUPT_ID_Msk;
        printk("irq pending! id: %x\r\n", interrupt_id);

        /* check if interrupt is in range */
        if (interrupt_id >= INTERRUPT_COUNT)
        {
            break;
        }

        /* allow preemption */
        enable_irqs();

        /* get matching handler for id */
        void(*handler)(void) = interrupt_handlers[interrupt_id];

        if (handler == NULL)
        {
            /* unhandled interrupt */
            while(1) {}
        }

        COMPLETE_MEMORY_READS;
        handler();
        COMPLETE_MEMORY_READS;

        /* turn off interrupts */
        disable_irqs();

        GIC_CPU->GICC_EOIR = current_interrupt;
    }
    COMPLETE_MEMORY_READS;
    enable_irqs();

    return;
}
