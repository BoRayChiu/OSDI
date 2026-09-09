#ifndef TASK_H
#define TASK_H

#include "string.h"
#include "uart.h"

#define MAX_TASKS 64
#define LSTACK_SIZE 4096
#define USTACK_SIZE 4096

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
    unsigned long sp; // Stack pointer; encoded as register number 31 in some instructions
};

struct trapframe {
    unsigned long x[31]; // General-purpose registers x0-x30
    unsigned long sp_el0;
    unsigned long elr_el1;
    unsigned long spsr_el1;
};

struct task {
    int taskid;
    enum task_state state;
    struct cpu_context context;
    void (*entry)(void);
    volatile int reschedled;
    struct trapframe *trapframe;
    int is_user;
};

extern void set_current(struct task *task);
extern struct task* get_current(void);
extern void switch_to(struct cpu_context *prev, struct cpu_context *next);
extern void enter_user(struct trapframe *tf, unsigned long kernel_stack_top);
extern void return_from_fork(void);

int privilege_task_create(void (*func)(void));
void task_init(void);
void context_switch(struct task *next);
void schedule(void);
void do_exec(void (*func)(void));
int do_fork(struct trapframe *parent_tf);

#endif // TASK_H