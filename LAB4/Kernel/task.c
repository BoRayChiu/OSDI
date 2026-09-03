#include "task.h"

int privilege_task_create(void (*func)(void)) {
    int id;
    for (id = 1; id < MAX_TASKS; id++) {
        if (task_pool[id].state == TASK_UNUSED) {
            break;
        }
    }

    if (id == MAX_TASKS) {
        return -1; // No available task slot
    }

    struct task *task = &task_pool[id];
    task->taskid = id;
    task->state = TASK_RUNNABLE;
    task->entry = func;

    // Clear the CPU context for the new task
    memzero(&task->context, sizeof(task->context));

    unsigned long stack_top = (unsigned long)&kstack_pool[id][LSTACK_SIZE];
    stack_top &= ~0xFUL; // Align to 16 bytes
    task->context.sp = stack_top;

    // Set the link register to the entry function
    task->context.lr = (unsigned long)func;

    return id;
}

void task_init() {
    for (int i = 0; i < MAX_TASKS; i++) {
        task_pool[i].taskid = i;
        task_pool[i].state = TASK_UNUSED;
    }
    task_pool[0].state = TASK_RUNNING;
    set_current(&task_pool[0]);
}

void context_switch(struct task *next) {
    struct task *prev = get_current();
    if (prev == next) {
        return;
    }
    prev->state = TASK_RUNNABLE;
    next->state = TASK_RUNNING;
    set_current(next);
    switch_to(&prev->context, &next->context);
}

struct task *get_task(int taskid) {
    if (taskid < 0 || taskid >= MAX_TASKS) {
        return 0;
    }
    return &task_pool[taskid];
}