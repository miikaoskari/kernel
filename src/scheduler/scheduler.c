#include "scheduler/scheduler.h"
#include "irq/irq.h"

static struct task_struct init_task = {
    .cpu_context = {0},
    .state = TASK_RUNNING,
    .counter = 0,
    .prio = 1,
    .preempt_count = 1,
};
struct task_struct *current = &init_task;
struct task_struct *task[TASK_COUNT] = {&init_task, };
int task_count = 1;


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

/**
 * @brief schedule the next task to run
 * 
 */
void _schedule()
{
    preempt_disable();
    int next, c;
    struct task_struct *p;

    while(1)
    {
        c = -1;
        next = 0;

        /* find the task with the highest priority */
        for (int i = 0; i < TASK_COUNT; i++)
        {
            p = task[i];
            if (p && p->state == TASK_RUNNING && p->counter > c)
            {
                c = p->counter;
                next = i;
            }
        }
        if (c)
        {
            break;
        }
        /* increment task counters, can never be larger than 2 * prio */
        for (int i = 0; i < TASK_COUNT; i++)
        {
            p = task[i];
            if (p)
            {
                p->counter = (p->counter >> 1) + p->prio;
            }
        }
    }
    switch_to(task[next]);
    preempt_enable();
}

/**
 * @brief schedule the next task to run
 * 
 */
void schedule()
{
    current->counter = 0;
    _schedule();
}

void schedule_tail()
{
    preempt_enable();
}

/**
 * @brief switch to the next task
 *
 * @param next the next task to switch to
 */
void switch_to(struct task_struct *next)
{
    if (current == next)
    {
        return;
    }

    struct task_struct *prev = current;
    current = next;
    cpu_switch_to(prev, next);
}

/**
 * @brief timer tick handler
 *
 */
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

