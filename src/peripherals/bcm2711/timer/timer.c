/**
 * @file timer.c
 * @brief Timer peripheral driver for BCM2711.
 *
 * This file contains the implementation of the timer peripheral driver
 * for the BCM2711. It provides functions to configure and use the
 * timer hardware.
 */
#include <stdint.h>
#include "peripherals/bcm2711/timer/timer.h"
#include "peripherals/bcm2711/bcm2711_lpa.h"
#include "scheduler/scheduler.h"
#include "utils/printk/printk.h"

const unsigned int interval = 200000;
unsigned int current_time = 0;
SYSTMR_Type timer;

/**
 * @brief Initializes the timer peripheral for the BCM2711.
 *
 * This function sets up the necessary configurations to initialize the timer
 * peripheral on the BCM2711. It ensures that the timer is ready for use and
 * properly configured according to the requirements of the system.
 */
void timer_init(void)
{
    current_time = timer.CLO;
    current_time += interval;
    timer.C1 = current_time;
}

/**
 * @brief Handles the timer interrupt request (IRQ).
 *
 * This function is called when a timer interrupt occurs. It processes the
 * interrupt and performs the necessary actions to handle the timer event.
 */
void handle_timer_irq(void)
{
    current_time += interval;
    timer.C1 = current_time;
    timer.CS = timer.CS_b.M1;
    printk("timer interrupt\n");
    timer_tick();
}
