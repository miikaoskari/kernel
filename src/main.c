#include <stddef.h>
#include <stdint.h>

#include "irq/irq.h"
#include "peripherals/uart/uart.h"
#include "peripherals/timer/timer.h"
#include "utils/printk/printk.h"

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

#define RP4 4

extern int get_el(void);
extern void enable_irq(void);
extern void irq_vector_init(void);

#ifdef AARCH64
// arguments for AArch64
void kmain(uint64_t dtb_ptr32, uint64_t x1, uint64_t x2, uint64_t x3)
#else
// arguments for AArch32
void kmain(uint32_t r0, uint32_t r1, uint32_t atags)
#endif
{
  uart_init(RP4);
  set_putc((putc_func_t)uart_putc);
  irq_vector_init();
  timer_init();
  enable_interrupt_controller();
  enable_irq();

  int el = get_el();
  printk("Current exception level: %d\n", el);

  while (1) {
    uart_putc(uart_getc());
  }
}
