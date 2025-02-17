#ifndef IRQ_H
#define IRQ_H

#include "peripherals/bcm2711/bcm2711_lpa.h"
#include <stdint.h>

void enable_interrupt_controller(void);
void show_invalid_entry_message(int type, unsigned long esr, unsigned long address);

void handle_irq(void);
void enable_irqs(void);
void disable_irqs(void);

void enable_irq(IRQn_Type irq);
extern void irq_vector_init(void);

#endif
