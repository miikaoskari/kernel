#include "scheduler/scheduler.h"
#include "peripherals/bcm2711/irq.h"

static struct task_struct init_task = {
    .cpu_context = {0},
    .state = TASK_RUNNING,
    .counter = 0,
    .prio = 1,
    .preempt_count = 1,
};
struct task_struct *current = &init_task;
struct task_struct *tasks[TASK_COUNT] = {&init_task, };

extern void enable_irq();
extern void disable_irq();

/**
 * @brief disable preemption for the current task
 * 
 */
void preempt_disable()
{
    current->preempt_count++;
}

/**
 * @brief enable preemption for the current task
 * 
 */
void preempt_enable()
{
    current->preempt_count--;
}

void _schedule()
{

}

void schedule()
{
    current->counter = 0;
    _schedule();
}

void timer_tick()
{
    current->counter--;
    if (current->counter > 0 || current->preempt_count > 0)
    {
        return;
    }

    current->counter = 0;

    enable_irq();
    _schedule();
    disable_irq();
}

