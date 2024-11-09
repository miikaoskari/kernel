#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#define THREAD_CPU_CONTEXT 0

#ifndef __ASSEMBLER__

/* thread */
#define THREAD_SIZE 4096

/* task states */
#define TASK_RUNNING 0
#define TASK_ZOMBIE 1
#define TASK_STOPPED 2
#define TASK_INTERRUPTIBLE 3
#define TASK_UNINTERRUPTIBLE 4

/* max task count */
#define TASK_COUNT 128

#define FIRST_TASK task[0]
#define LAST_TASK task[TASK_COUNT - 1]

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

void preempt_disable();
void preempt_enable();
void schedule();
void timer_tick();
#ifndef __ASSEMBLER__
void schedule_tail();
#endif

extern struct task_struct *current;
extern struct task_struct *task[TASK_COUNT];
extern int task_count;

/* asm */
extern void cpu_switch_to(struct task_struct *prev, struct task_struct *next);

#endif
#endif
