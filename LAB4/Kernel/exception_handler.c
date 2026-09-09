#include "exception_handler.h"

void synchronous_exception_handler(unsigned long elr, unsigned long esr) {
    unsigned long ec = (esr >> 26) & 0x3F;
    unsigned long iss = esr & 0x1FFFFFF;
    uart_send_string("\r\nException return address: 0x");
    uart_send_string(itoa(elr, 16));
    uart_send_string("\r\nException class(EC): 0x");
    uart_send_string(itoa(ec, 16));
    uart_send_string("\r\nInstruction specific syndrome(ISS): 0x");
    uart_send_string(itoa(iss, 16));
}

void el0_synchronous_exception_handler(struct trapframe *tf, unsigned long esr) {
    unsigned long ec = (esr >> 26) & 0x3F;
    unsigned long iss = esr & 0x1FFFFFF;

    if (ec != EC_SVC64) {
        uart_send_string("Unknown synchronous exception\r\n");
        return;
    }

    unsigned long syscall_num = tf->x[8]; // x8 holds the syscall number
    switch (syscall_num) {
        case SYS_UART_WRITE:
            tf->x[0] = sys_uart_write((const char *)tf->x[0], tf->x[1]);
            break;

        case SYS_UART_READ:
            tf->x[0] = sys_uart_read((char *)tf->x[0], tf->x[1]);
            break;

        case SYS_EXEC:
            void (*func)(void) = (void (*)(void))tf->x[0];
            do_exec(func);
            break;

        case SYS_FORK:
            int child_id = do_fork(tf);
            tf->x[0] = child_id;
            break;

        case SYS_EXIT:
            uart_send_string("[System call] svc #4\r\n");
            break;

        case SYS_ENABLE_TIMER:
            uart_send_string("[System call] Enable core timer\r\n");
            core_timer_enable();
            break;

        default:
            uart_send_string("Unknown system call");
            uart_send_string(itoa(syscall_num, 10));
            uart_send_string("\r\n");
            tf->x[0] = -1; // Return -1 for unknown syscall
            break;
    }
}


void irq_exception_handler() {
    unsigned int source = CORE0_IRQ_SOURCE;
    
    // ARM Core Timer
    if (source & (1u << 1)) {
        core_timer_handler();
       
        struct task *cur = get_current();
        if (cur != 0) {
            cur->reschedled = 1;
        }
    }

    // ARM Local Timer
    if (source & (1u << 11)) {
        local_timer_handler();
        uart_send_string("[Local timer IRQ]\r\n");
    }

    // GPU peripheral interrupt
    if (source & (1u << 8)) {
        if (*IRQ_PENDING_2 & (1u << 25)) {
            uart_irq_handler();
        }
    }
}

void el0_irq_exception_handler(struct trapframe *tf) {
    struct task *task = get_current();
    task->trapframe = tf;
    irq_exception_handler();
}