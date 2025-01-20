#include <stddef.h>
#include <stdint.h>

#include "irq/irq.h"
#include "peripherals/bcm2711/uart/uart.h"
#include "peripherals/bcm2711/timer/timer.h"
#include "scheduler/fork.h"
#include "scheduler/scheduler.h"
#include "utils/printk/printk.h"
#include "delay/delay.h"
#include "mem/mmu.h"

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

#define RP4 4

extern int get_el(void);

void process1(char *array)
{
  printk("Process %s started\n", array);
  while (1)
  {
    printk("Process %s running\n", array);
    delay(10);
  }
}

#ifdef AARCH64
// arguments for AArch64
void kmain(uint64_t dtb_ptr32, uint64_t x1, uint64_t x2, uint64_t x3)
#else
// arguments for AArch32
void kmain(uint32_t r0, uint32_t r1, uint32_t atags)
#endif
{
  mmu_init();

  uart_init(RP4);
  set_putc((putc_func_t)uart_putc);
  irq_vector_init();
  timer_init();

  // enable gic
  enable_irqs();
  enable_interrupt_controller();

  int el = get_el();
  printk("Current exception level: %d\n", el);

  int ret = copy_process((unsigned long)&process1, (unsigned long)"test");
  if (ret)
  {
    printk("Failed to create process\n");
  }

  ret = copy_process((unsigned long)&process1, (unsigned long)"test");
  if (ret)
  {
    printk("Failed to create process\n");
  }

  while (1) {
    schedule();
  }
}
