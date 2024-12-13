#ifndef IRQ_H
#define IRQ_H

void enable_interrupt_controller();
void show_invalid_entry_message(int type, unsigned long esr, unsigned long address);
void handle_irq();

void enable_irq();
void enable_with_daif();
void disable_with_daif();
extern void irq_vector_init();

#endif
