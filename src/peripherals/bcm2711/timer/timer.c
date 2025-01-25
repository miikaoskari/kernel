#include <stdint.h>
#include "peripherals/bcm2711/timer/timer.h"
#include "peripherals/bcm2711/bcm2711_lpa.h"
#include "scheduler/scheduler.h"
#include "utils/printk/printk.h"

const unsigned int interval = 200000;
unsigned int current_time = 0;
SYSTMR_Type timer;

void timer_init(void)
{
    current_time = timer.CLO;
    current_time += interval;
    timer.C1 = current_time;
}

void handle_timer_irq(void)
{
    current_time += interval;
    timer.C1 = current_time;
    timer.CS = timer.CS_b.M1;
    printk("timer interrupt\n");
    timer_tick();
}
