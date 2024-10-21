#ifndef _SCHEDULER_H
#define _SCHEDULER_H

/**
 * @brief cpu context structure
 * only save x19-x28, fp, sp, pc because x0-x18 can be 
 * overwritten by the called function
 */
struct cpu_context {
    unsigned long x19;
    unsigned long x20;
    unsigned long x21;
    unsigned long x22;
    unsigned long x23;
    unsigned long x24;
    unsigned long x25;
    unsigned long x26;
    unsigned long x27;
    unsigned long x28;
    unsigned long fp; /* frame pointer */
    unsigned long sp; /* stack pointer */
    unsigned long pc; /* program counter */
};

struct task_struct {
    struct cpu_context cpu_context;
    long state; /* state of current task */
    long counter; /* how long the task has been running -1 per tick */
    long prio; /* task priority copied over to counter. regulate cpu time */
    long preempt_count; /* 0 = preemptable, <0 = not preemptable */
};

#endif