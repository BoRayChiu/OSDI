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

void el0_synchronous_exception_handler(unsigned long esr) {
    unsigned long ec;
    unsigned long iss;
    unsigned long syscall_num;

    ec  = (esr >> 26) & 0x3f;
    iss = esr & 0x1ffffff;

    if (ec == EC_SVC64) {
        syscall_num = iss & 0xffff;

        if (syscall_num == SYSCALL_ENABLE_TIMER) {
            uart_send_string("[System call] Enable core timer\r\n");
            core_timer_enable();
            return;
        }
        if (syscall_num == SYSCALL_EXC) {
            uart_send_string("[System call] svc #1\r\n");
            return;
        }
        uart_send_string("Unknown system call\r\n");
        return;
    }
    uart_send_string("Unknown synchronous exception\r\n");
}


void irq_exception_handler() {
    unsigned int source = CORE0_IRQ_SOURCE;
    
    // ARM Core Timer
    if (source & (1u << 1)) {
        core_timer_handler();
        uart_send_string("[Core timer IRQ]\r\n");
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