#include "peripherals/bcm2711/timer/timer.h"
#include "mmio/mmio.h"
#include "utils/printk/printk.h"

const unsigned int interval = 200000;
unsigned int current_time = 0;

enum
{
    TIMER_BASE = 0x7E003000,

    TIMER_CS = (TIMER_BASE + 0x00000000),
    TIMER_CLO = (TIMER_BASE + 0x00000004),
    TIMER_CHI = (TIMER_BASE + 0x00000008),
    TIMER_C0 = (TIMER_BASE + 0x0000000C),
    TIMER_C1 = (TIMER_BASE + 0x00000010),
    TIMER_C2 = (TIMER_BASE + 0x00000014),
    TIMER_C3 = (TIMER_BASE + 0x00000018),

    TIMER_CS_M0 = (1 << 0),
    TIMER_CS_M1 = (1 << 1),
    TIMER_CS_M2 = (1 << 2),
    TIMER_CS_M3 = (1 << 3),
};

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
