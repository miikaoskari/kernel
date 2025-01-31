/**
 * @file main.c
 * @brief Main source file for the kernel.
 *
 * This file contains the main entry point and initialization code for the kernel.
 * It includes the necessary standard library headers.
 */
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

/**
 * @brief Test process for the kernel.
 *
 * @param array A pointer to the character array to be processed.
 */
void process1(char *array)
{
  printk("Process %s started\n", array);
  while (1)
  {
    printk("Process %s running\n", array);
    delay(10);
  }
}

/**
 * @brief Kernel main entry point for different architectures.
 *
 * This function serves as the entry point for the kernel, with different
 * arguments depending on the architecture (AArch64 or AArch32).
 *
 * @param dtb_ptr32 Device Tree Blob pointer (AArch64 only).
 * @param x1 General purpose register 1 (AArch64 only).
 * @param x2 General purpose register 2 (AArch64 only).
 * @param x3 General purpose register 3 (AArch64 only).
 * @param r0 General purpose register 0 (AArch32 only).
 * @param r1 General purpose register 1 (AArch32 only).
 * @param atags Address of the tagged list (AArch32 only).
 */
#ifdef AARCH64
// arguments for AArch64
void kmain(uint64_t dtb_ptr32, uint64_t x1, uint64_t x2, uint64_t x3)
#else
// arguments for AArch32
void kmain(uint32_t r0, uint32_t r1, uint32_t atags)
#endif
{
  setup_mmu_flat_map();

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

/**
 * @brief Function to spin the CPU1.
 *
 * This function is responsible for spinning the CPU1.
 */
void spin_cpu1(void)
{
    while (1);
}

/**
 * @brief Function to spin the CPU2.
 *
 * This function is responsible for spinning the CPU2.
 */
void spin_cpu2(void)
{
    while (1);
}

/**
 * @brief Function to spin the CPU3.
 *
 * This function is responsible for spinning the CPU3.
 */
void spin_cpu3(void)
{
    while (1);
}
