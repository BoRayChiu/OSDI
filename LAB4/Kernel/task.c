#include "task.h"

void enqueue_task(struct task *task) {
    if (rq_count >= MAX_TASKS) {
        return;
    }

    runqueue[rq_tail] = task;
    rq_tail = (rq_tail + 1) % MAX_TASKS;
    rq_count++;
}

struct task *dequeue_task() {
    if (rq_count == 0) {
        return 0;
    }

    struct task *task = runqueue[rq_head];
    rq_head = (rq_head + 1) % MAX_TASKS;
    rq_count--;
    return task;
}

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

    enqueue_task(task);

    return id;
}

void task_init() {
    for (int i = 0; i < MAX_TASKS; i++) {
        task_pool[i].taskid = i;
        task_pool[i].state = TASK_UNUSED;
        task_pool[i].reschedled = 0;
    }
    task_pool[0].state = TASK_RUNNING;
    task_pool[0].reschedled = 0;
    set_current(&task_pool[0]);
}

void context_switch(struct task *next) {
    struct task *prev = get_current();
    if (prev == next) {
        return;
    }
    set_current(next);
    switch_to(&prev->context, &next->context);
}

void schedule() {
    struct task *prev = get_current();
    struct task *next;
    if (prev->state == TASK_RUNNING && prev->taskid != 0) {
        prev->state = TASK_RUNNABLE;
        enqueue_task(prev);
    }
    next = dequeue_task();
    if (next == 0) {
        next = &task_pool[0]; // Fallback to the idle task
    }
    if (next == prev) {
        return; // No need to switch if the next task is the same as the current task
    }
    next->state = TASK_RUNNING;
    if (prev->taskid == 0) {
        prev->state = TASK_RUNNABLE; // Set the idle task back to runnable
    }
    context_switch(next);
}

void do_exec(void (*func)(void)) {
    struct task *task = get_current();
    unsigned long kstack_top = (unsigned long)&kstack_pool[task->taskid][LSTACK_SIZE];
    kstack_top &= ~0xFUL; // Align to 16 bytes
    // Set up the trapframe at the top of the kernel stack
    struct trapframe *tf = (struct trapframe *)(kstack_top - sizeof(struct trapframe));

    memzero(tf, sizeof(struct trapframe));

    unsigned long ustack_top = (unsigned long)&ustack_pool[task->taskid][USTACK_SIZE];
    ustack_top &= ~0xFUL; // Align to 16 bytes
    tf->sp_el0 = ustack_top;
    tf->elr_el1 = (unsigned long)func;
    tf->spsr_el1 = 0x0; // EL0t, DAIF = 0, interrupts enabled

    task->trapframe = tf;
    task->user_stack_top = ustack_top;
    task->is_user = 1;

    // Switch to user mode and start executing the function
    enter_user(tf, kstack_top);

    while (1) {
        // This point should never be reached if the user function returns
        // If it does, we can just halt the CPU or reset the system
        asm volatile("wfi");
    }
}

void check_reschedule() {
    struct task *task = get_current();

    if (task->is_user && task->reschedled) {
        uart_send_string("[Preempt task ");
        uart_send_string(itoa(task->taskid, 10));
        uart_send_string("]\r\n");

        task->reschedled = 0;
        schedule();
    }
}