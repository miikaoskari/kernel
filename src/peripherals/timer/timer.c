#include "peripherals/timer/timer.h"
#include "mmio/mmio.h"
#include "utils/printk/printk.h"

const unsigned int interval = 200000;
unsigned int current_time = 0;

void timer_init(void)
{
    current_time = mmio_read(TIMER_CLO);
    current_time += interval;
    mmio_write(TIMER_C1, current_time);
}

void handle_timer_irq(void)
{
    current_time += interval;
    mmio_write(TIMER_C1, current_time);
    mmio_write(TIMER_CS, TIMER_CS_M1);
    printk("timer interrupt\n");
}
