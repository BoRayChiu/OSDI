#include "uart.h"
#include "mailbox.h"

static void wait_cycles(int cycles) {
    for (int i = 0; i < cycles; i++) {
        asm volatile("nop");
    }
}

void uart_init() {
    // 0. clear UART_CR
    *UART_CR = 0;

    // 1.
    mailbox[0] = 9 * 4;
    mailbox[1] = REQUEST_CODE;
    mailbox[2] = SET_CLOCK_RATE;
    mailbox[3] = 12;
    mailbox[4] = 0;
    mailbox[5] = 2; // UART clock ID
    mailbox[6] = 4000000;
    mailbox[7] = 0;
    mailbox[8] = END_TAG;

    mailbox_call(8);

    // 2.
    *GPFSEL1 &= ~((7 << 12) | (7 << 15));
    *GPFSEL1 |= (4 << 12) | (4 << 15);
    *GPPUD = 0;
    wait_cycles(150);
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    wait_cycles(150);
    *GPPUD = 0;
    *GPPUDCLK0 = 0;
 
    // 3.
    *UART_ICR = 0x7FF; // clear all interrupts
    *UART_IBRD = 2;
    *UART_FBRD = 11;

    // 4.
    *UART_LCRH = 0x70;

    // 5.
    *UART_CR = 0x301;
}

void uart_send(unsigned int c) {
    while (*UART_FR & 0x20) wait_cycles(1);
    *UART_DR = c;
}

char uart_recv() {
    while (*UART_FR & 0x10) wait_cycles(1);
    return (char)(*UART_DR);
}

void uart_send_string(const char* str) {
    while (*str) {
        uart_send(*str++);
    }
}