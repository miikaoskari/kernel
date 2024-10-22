#include "scheduler/scheduler.h"

static struct task_struct init_task = {
    .cpu_context = {0},
    .state = TASK_RUNNING,
    .counter = 0,
    .prio = 1,
    .preempt_count = 1,
};
struct task_struct *current = &init_task;
struct task_struct *tasks[TASK_COUNT] = {&init_task, };

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

void schedule()
{

}

void schedule_tail()
{

}

