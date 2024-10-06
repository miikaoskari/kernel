#include "mmio/mmio.h"
#include "utils/printk/printk.h"
#include "peripherals/bcm2711/irq.h"
#include "peripherals/bcm2711/timer/timer.h"

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

void enable_interrupt_controller()
{
    /* enable system timer irq */
    mmio_write(ENABLE_IRQS_1, SYSTEM_TIMER_IRQ_1);
}

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address)
{
    printk("%s, ESR: %x, address: %x\r\n", entry_error_messages[type], esr, address);
}

void handle_irq()
{
    unsigned int irq = mmio_read(IRQ_PENDING_1);
    switch (irq)
    {
    case (SYSTEM_TIMER_IRQ_1):
        handle_timer_irq();
        break;
    default:
        printk("Unknown pending irq: %x\r\n", irq);
        break;
    }
}
