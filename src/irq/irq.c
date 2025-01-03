#include <stdint.h>
#include <stdbool.h>
#include "mmio/mmio.h"
#include "utils/printk/printk.h"
#include "peripherals/bcm2711/timer/timer.h"
#include "peripherals/bcm2711/bcm2711_lpa.h"
#include "peripherals/bcm2711/core_ca72.h"

static IRQn_Type _current_interrupt;

const char *entry_error_messages[] = {
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
static uint8_t get_current_cpu()
{
    uint32_t mpidr = 0;
    __asm__("mrs     %[mpidr], mpidr_el1"
            : /* No outputs. */
            : [mpidr] "r" (mpidr));
    return mpidr & 0xff;
}

/* enable cpu local irqs */
void enable_irqs()
{
    /* [0..0] Enable signaling of group 0 */
    GIC_CPU->GICC_CTLR_b.ENABLE_GROUP_0 = true;

    /* enable irqs */
    asm volatile("msr   daifclr, #2");
    asm volatile("isb");
    return;
}

void disable_irq()
{
    /* disable IRQ with daifset */
    asm volatile("msr   daifset, #2");
    asm volatile("isb");
    return; 
}

void enable_interrupt_controller()
{
    /* enable system timer irq */
    uint8_t cpu_id = get_current_cpu();
}

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address)
{
    printk("%s, ESR: %x, address: %x\r\n", entry_error_messages[type], esr, address);
}

void handle_irq()
{
    /* Get the interrupt acknowledge register.
     * Register changes state after reading. */
    uint32_t current_interrupt = GIC_CPU->GICC_IAR;

    /* Get id of the current interrupt */
    uint32_t interrupt_id = current_interrupt & ARM_GIC400_CPU_GICC_IAR_INTERRUPT_ID_Msk;
    printk("irq pending! id: %x\r\n", interrupt_id);



}
