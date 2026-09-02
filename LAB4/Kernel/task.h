#ifndef TASK_H
#define TASK_H

#define MAX_TASKS 64
#define LSTACK_SIZE 4096

enum task_state {
    TASK_UNUSED,
    TASK_RUNNABLE,
    TASK_RUNNING,
};

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
    unsigned long fp; // Frame pointer (x29)
    unsigned long lr; // Link register (x30)
    unsigned long sp; // Stack pointer (x31)
};

struct task {
    int taskid;
    enum task_state state;
    struct cpu_context context;
    void (*entry)(void);
};

static struct task task_pool[MAX_TASKS];
static unsigned char kstack_pool[MAX_TASKS][LSTACK_SIZE]
    __attribute__((aligned(16)));

int privilege_task_create(void (*func)(void));

#endif // TASK_H