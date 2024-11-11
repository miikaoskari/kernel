#include "scheduler/fork.h"
#include "scheduler/scheduler.h"
#include "mem/mem.h"
#include "entry.h"

/**
 * @brief Create a new process
 *
 * @param fn Function to run
 * @param arg Argument to pass to the function
 *
 * @return int 0 if successful, 1 if not
 */
int copy_process(unsigned long fn, unsigned long arg)
{
    preempt_disable();

    struct task_struct *p;

    p = (struct task_struct *) get_free_page();
    if (!p)
    {
        return 1;
    }

    p->prio = current->prio;
    p->state = TASK_RUNNING;
    p->counter = p->prio;
    p->preempt_count = 1;

    p->cpu_context.x19 = fn;
    p->cpu_context.x20 = arg;
    p->cpu_context.pc = (unsigned long)ret_from_fork;
    p->cpu_context.sp = (unsigned long)p + THREAD_SIZE;

    int pid = task_count++;
    task[pid] = p;
    preempt_enable();
    return 0;
}

