#include "task.h"

static struct task *runqueue[MAX_TASKS];
static int rq_head = 0;
static int rq_tail = 0;
static int rq_count = 0;
static struct task task_pool[MAX_TASKS];
static unsigned char kstack_pool[MAX_TASKS][LSTACK_SIZE]
    __attribute__((aligned(16)));
static unsigned char ustack_pool[MAX_TASKS][USTACK_SIZE]
    __attribute__((aligned(16)));

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

    int best_priority = -1;
    int best_offset = -1;

    for (int i = 0; i < rq_count; i++) {
        int idx = (rq_head + i) % MAX_TASKS;
        struct task *task = runqueue[idx];
        if (task->priority > best_priority) {
            best_priority = task->priority;
            best_offset = i;
        }
    }

    int best_idx = (rq_head + best_offset) % MAX_TASKS;
    struct task *best = runqueue[best_idx];

    for (int i = best_offset; i < rq_count - 1; i++) {
        int from = (rq_head + i + 1) % MAX_TASKS;
        int to = (rq_head + i) % MAX_TASKS;
        runqueue[to] = runqueue[from];
    }

    rq_tail = (rq_tail - 1 + MAX_TASKS) % MAX_TASKS;
    rq_count--;
    return best;
}

int privilege_task_create_priority(void (*func)(void), int priority) {
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
    task->reschedled = 0;
    task->trapframe = 0;
    task->is_user = 0;
    task->exit_status = 0;
    task->pending_signals = 0;
    task->priority = priority;

    // Clear the CPU context for the new task
    memzero(&task->context, sizeof(task->context));

    unsigned long stack_top = (unsigned long)&kstack_pool[id][LSTACK_SIZE];
    stack_top &= ~0xFUL; // Align to 16 bytes
    task->context.sp = stack_top - sizeof(struct trapframe);

    // Set the link register to the entry function
    task->context.lr = (unsigned long)func;

    enqueue_task(task);

    return id;
}

int privilege_task_create(void (*func)(void)) {
    return privilege_task_create_priority(func, PRIORITY_NORMAL);
}

void task_init() {
    for (int i = 0; i < MAX_TASKS; i++) {
        task_pool[i].taskid = i;
        task_pool[i].state = TASK_UNUSED;
        task_pool[i].reschedled = 0;
        task_pool[i].pending_signals = 0;
        task_pool[i].priority = PRIORITY_NORMAL;
    }
    task_pool[0].state = TASK_RUNNING;
    task_pool[0].reschedled = 0;
    task_pool[0].priority = PRIORITY_LOWEST;
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
    next->state = TASK_RUNNING;
    if (next == prev) {
        return; // No need to switch if the next task is the same as the current task
    }
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

int do_fork(struct trapframe *parent_tf) {
    struct task *parent = get_current();

    // Find empty Task Slot from Task Pool
    int id;
    for (id = 1; id < MAX_TASKS; id++) {
        if (task_pool[id].state == TASK_UNUSED) {
            break;
        }
    }

    if (id == MAX_TASKS) {
        return -1; // No available task slot
    }

    // Initialize Child Task
    struct task *child = &task_pool[id];
    child->taskid = id;
    child->state = TASK_RUNNABLE;
    child->entry = 0;
    child->reschedled = 0;
    child->is_user = 1;
    child->pending_signals = 0;
    child->priority = parent->priority;
    memzero(&child->context, sizeof(child->context));

    // Construct Child Trapframe
    unsigned long child_kstack_top = (unsigned long)&kstack_pool[id][LSTACK_SIZE];
    child_kstack_top &= ~0xFUL; // Align to 16 bytes
    struct trapframe *child_tf = (struct trapframe *)(child_kstack_top - sizeof(struct trapframe));
    memcopy(child_tf, parent_tf, sizeof(struct trapframe));

    // Copy User Stack
    unsigned long parent_ubase = (unsigned long) &ustack_pool[parent->taskid][0];
    unsigned long child_ubase = (unsigned long) &ustack_pool[id][0];
    memcopy((void *)child_ubase, (void *)parent_ubase, USTACK_SIZE);

    // Set Child SP_EL0
    unsigned long sp_offset = parent_tf->sp_el0 - parent_ubase;
    child_tf->sp_el0 = child_ubase + sp_offset;

    // Set Child Frame Pointer (x29)
    unsigned long parent_utop = (unsigned long) &ustack_pool[parent->taskid][USTACK_SIZE];
    if (parent_tf->x[29] >= parent_ubase && parent_tf->x[29] <= parent_utop) {
        unsigned long fp_offset = parent_tf->x[29] - parent_ubase;
        child_tf->x[29] = child_ubase + fp_offset;
    }

    // Child return 0
    child_tf->x[0] = 0;

    child->trapframe = child_tf;
    child->context.sp = (unsigned long)child_tf;
    child->context.lr = (unsigned long)return_from_fork;
    child->exit_status = 0;

    enqueue_task(child);

    // Parent return Child id
    return id;
}

void do_exit(int status) {
    struct task *task = get_current();

    // Idle Task could not exit
    if (task->taskid == 0) {
        uart_send_string("[Error] Idle Task tried to exit\r\n");
        while(1);
    }

    task->exit_status = status;
    task->reschedled = 0;
    task->state = TASK_ZOMBIE;

    uart_send_string("[Exit] task ");
    uart_send_string(itoa(task->taskid, 10));
    uart_send_string(", status ");
    uart_send_string(itoa(status, 10));
    uart_send_string("\r\n");

    schedule();

    while(1);
}

static void reap_zombies() {
    for (int i = 1; i < MAX_TASKS; i++) {
        struct task *task = &task_pool[i];
        if (task->state != TASK_ZOMBIE) {
            continue;
        }
        uart_send_string("[Reaper] Reclaim Task ");
        uart_send_string(itoa((const unsigned long)task->taskid, 10));
        uart_send_string(", exit status ");
        uart_send_string(itoa((const unsigned long)task->exit_status, 10));
        uart_send_string("\r\n");

        // Clear Zombie Task Stack
        memzero(kstack_pool[i], LSTACK_SIZE);
        memzero(ustack_pool[i], USTACK_SIZE);
        memzero(&task->context, sizeof(task->context));
        task->entry = 0;
        task->reschedled = 0;
        task->trapframe = 0;
        task->is_user = 0;
        task->exit_status = 0;
        task->pending_signals = 0;
        task->state = TASK_UNUSED;
    }
}

void zombie_reaper() {
    while (1) {
        reap_zombies();
        schedule();
    }
}

int do_kill(int pid, int signal) {
    if (signal != SIGKILL) {
        return -1;
    }

    if (pid < 0 || pid >= MAX_TASKS) {
        return -1;
    }

    struct task *task = &task_pool[pid];
    if (task->state == TASK_UNUSED) {
        return -1;
    }

    if (task->state == TASK_ZOMBIE) {
        return -1;
    }

    // Can not kill Privilege Task
    if (!task->is_user) {
        return -1;
    }

    task->pending_signals |= SIG_MASK(SIGKILL);
    return 0;
}

void check_pending_signal() {
    struct task *task = get_current();
    if (!task->is_user) {
        return;
    }

    if (task->pending_signals & SIG_MASK(SIGKILL)) {
        task->pending_signals &= ~SIG_MASK(SIGKILL);
        uart_send_string("[Signal] Task");
        uart_send_string(itoa(task->taskid, 10));
        uart_send_string(" received SIGKILL\r\n");
        // Define SIGKILL exit status 9
        do_exit(SIGKILL);
    }
    
}

static struct task *uart_wait_queue[MAX_TASKS];
static int uart_wait_head = 0;
static int uart_wait_tail = 0;
static int uart_wait_count = 0;

static void uart_wait_enqueue(struct task *task) {
    if (uart_wait_count >= MAX_TASKS) {
        return;
    }
    uart_wait_queue[uart_wait_tail] = task;
    uart_wait_tail = (uart_wait_tail + 1) % MAX_TASKS;
    uart_wait_count++;
}

static struct task *uart_wait_dequeue() {
    if (uart_wait_count == 0) {
        return 0;
    }
    struct task *task = uart_wait_queue[uart_wait_head];
    uart_wait_head = (uart_wait_head + 1) % MAX_TASKS;
    uart_wait_count--;
    return task;
}

void wake_one_uart_waiter() {
    struct task *task = uart_wait_dequeue();
    if (task == 0) {
        return;
    }
    if (task->state != TASK_WAITING) {
        return;
    }
    task->state = TASK_RUNNABLE;
    enqueue_task(task);
}

void block_current_on_uart() {
    struct task *task = get_current();
    task->state = TASK_WAITING;
    uart_wait_enqueue(task);
}

static void mutex_wait_enqueue(struct mutex *m, struct task *task) {
    if (m->wait_count >= MAX_TASKS) {
        return;
    }
    m->wait_queue[m->wait_tail] = task;
    m->wait_tail = (m->wait_tail + 1) % MAX_TASKS;
    m->wait_count++;
}

static struct task *mutex_wait_dequeue(struct mutex *m) {
    if (m->wait_count == 0) {
        return 0;
    }
    struct task *task = m->wait_queue[m->wait_head];
    m->wait_head = (m->wait_head + 1) % MAX_TASKS;
    m->wait_count--;
    return task;
}

void mutex_init(struct mutex *m) {
    m->locked = 0;
    m->owner = 0;
    m->wait_head = 0;
    m->wait_tail = 0;
    m->wait_count = 0;
}

void mutex_lock(struct mutex *m) {
    while (1) {
        disable_irq_el1();
        struct task *task = get_current();
        if (!m->locked) {
            m->locked = 1;
            m->owner = task;
            enable_irq_el1();
            return;
        }
        task->state = TASK_WAITING;
        mutex_wait_enqueue(m, task);
        enable_irq_el1();
        schedule();
    }
}

void mutex_unlock(struct mutex *m) {
    disable_irq_el1();
    struct task *current = get_current();
    if (!m->locked || m->owner != current) {
        enable_irq_el1();
        return;
    }
    m->locked = 0;
    m->owner = 0;
    struct task *task = mutex_wait_dequeue(m);
    if (task != 0 && task->state == TASK_WAITING) {
        task->state = TASK_RUNNABLE;
        enqueue_task(task);
    }
    enable_irq_el1();
}
