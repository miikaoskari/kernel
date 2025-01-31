/**
 * @file scheduler.c
 * @brief Implementation of the scheduler module.
 *
 * This file contains the implementation of the scheduler functions
 * which are responsible for managing task scheduling in the kernel.
 */
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
 * @brief Disables preemption.
 *
 * This function disables the preemption mechanism, preventing the current
 * task from being preempted by other tasks.
 */
void preempt_disable()
{
    current->preempt_count++;
}

/**
 * @brief Enables preemption by the scheduler.
 *
 * This function re-enables preemption, allowing the scheduler to
 * interrupt the current task and switch to another task if necessary.
 */
void preempt_enable()
{
    current->preempt_count--;
}

/**
 * @brief Internal function to handle the scheduling of tasks.
 *
 * This function is responsible for managing the scheduling of tasks within the kernel.
 * It is called internally by the scheduler to determine which task should be executed next.
 *
 * @note This function should not be called directly. It is intended for internal use by the scheduler.
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
 * @brief Selects the next process to run and performs a context switch.
 *
 * This function is responsible for determining the next process to run based on the
 * scheduling algorithm implemented. It performs the necessary context switch to
 * transfer control to the selected process.
 */
void schedule()
{
    current->counter = 0;
    _schedule();
}

/**
 * @brief Finalizes the scheduling process for the current task.
 *
 * This function is called at the end of the scheduling process to perform any
 * necessary cleanup or finalization steps for the current task. It ensures
 * that the task is properly set up to continue execution.
 */
void schedule_tail()
{
    preempt_enable();
}


/**
 * @brief Switches the CPU context to the next task.
 * 
 * This function is responsible for switching the CPU context from the
 * currently running task to the next task specified by the @next parameter.
 * It performs the necessary operations to save the state of the current task
 * and restore the state of the next task, allowing the scheduler to manage
 * task execution efficiently.
 * 
 * @param next The next task to switch to.
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
 * @brief Handles the timer tick event.
 *
 * This function is called on each timer tick to perform necessary
 * scheduling operations. It is responsible for updating the system
 * timer and managing task scheduling.
 */
void timer_tick(void)
{
    current->counter--;
    if (current->counter > 0 || current->preempt_count > 0)
    {
        return;
    }

    current->counter = 0;

    enable_irqs();
    _schedule();
    disable_irqs();
}
