#include "mmio/mmio.h"
#include "utils/printk/printk.h"
#include "peripherals/bcm2711/timer/timer.h"
#include "peripherals/bcm2711/irq.h"
#include "peripherals/bcm2711/bcm2711_lpa.h"
#include "peripherals/bcm2711/core_ca72.h"
#include <stdint.h>


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

void enable_irq(unsigned int interrupt_id)
{
    /* The corresponding GICD_ISENABLER<n> number, n, is given by n = m DIV 32. */
    unsigned int n = interrupt_id / 32;
    /* The offset of the required GICD_ISENABLER is (0x100 + (4*n)). */
    unsigned int register_offset = GICD_ISENABLER_BASE + (4 * n);
    /* The bit number of the required interrupt is m MOD 32. */
    unsigned int bit_number = interrupt_id % 32;
    /* Set the corresponding bit in the GICD_ISENABLER<n> register. */
    mmio_write(register_offset, 1 << bit_number);
}

/* For interrupt ID m, when DIV and MOD are the integer division and modulo operations. */
void assign_target_cpu(unsigned int interrupt_id, unsigned int cpu_id)
{
    /* enable IRQ with daifclr */
    asm volatile("msr daifclr, #2");
    /* The corresponding GICD_ITARGETSR<n> number, n, is given by n = m DIV 4. */
    unsigned int n = interrupt_id / 4;
    /* The offset of the required GICD_ITARGETSR<n> register is (0x800 + (4*n)) */
    unsigned int register_offset = GICD_ITARGETSR_BASE + (4 * n);
    /* Get current value of the register */
    unsigned int current_register_value = mmio_read(register_offset);
    /* Set the correct CPU target */
    mmio_write(register_offset, current_register_value | 1 << cpu_id);
}

void disable_irq()
{
    /* disable IRQ with daifset */
    asm volatile("msr daifset, #2");
    return; 
}

void enable_interrupt_controller()
{
    /* enable system timer irq */
    uint8_t cpu_id = get_current_cpu();
    assign_target_cpu(SYSTEM_TIMER_IRQ_1, cpu_id);
    enable_irq(SYSTEM_TIMER_IRQ_1);
}

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address)
{
    printk("%s, ESR: %x, address: %x\r\n", entry_error_messages[type], esr, address);
}

void handle_irq()
{
    /* Get the interrupt acknowledge register */
    unsigned int irq_ack = mmio_read(GICC_IAR);
    /* Get the interrupt ID
     * INTIDs 1020-1023 are reserved and convey additional information
     * such as spurious interrupts. */
    unsigned int irq = irq_ack & 0x3ff;
    printk("irq pending: %x\r\n", irq);
    switch (irq)
    {
    case (SYSTEM_TIMER_IRQ_1):
        handle_timer_irq();
        mmio_write(GICC_EOIR, irq_ack);
        break;
    default:
        printk("Unknown pending irq: %x\r\n", irq);
        break;
    }
}
