#include "uart.h"
#include "mailbox.h"

#define UART_RX_BUFFER_SIZE 256

static volatile char uart_rx_buffer[UART_RX_BUFFER_SIZE];
static volatile unsigned int uart_rx_head;
static volatile unsigned int uart_rx_tail;

static void wait_cycles(int cycles) {
    for (int i = 0; i < cycles; i++) {
        asm volatile("nop");
    }
}

void uart_init() {
    uart_rx_head = 0;
    uart_rx_tail = 0;

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
    // Polling version
    while (*UART_FR & 0x10) wait_cycles(1);
    return (char)(*UART_DR);
    /*
    // Interrupt version
    while (uart_rx_head == uart_rx_tail) wait_cycles(1);
    char c = uart_rx_buffer[uart_rx_tail];
    uart_rx_tail = (uart_rx_tail + 1) & (UART_RX_BUFFER_SIZE - 1);
    return c;
    */
}

void uart_send_string(const char* str) {
    while (*str) {
        uart_send(*str++);
    }
}

void uart_irq_init(void) {
    uart_rx_head = 0;
    uart_rx_tail = 0;

    *UART_ICR = 0x7FF;

    /*
     * Enable
     * RX interrupt
     * RT interrupt
     * Receive Errot interrupt
    */
    *UART_IMSC |= UART_INT_RX | UART_INT_RT | UART_INT_ERR;

    *IRQ_ENABLE_2 = (1u << 25);
    *GPU_INTERRUPTS_ROUTING = 0;
}

void uart_irq_handler(void) {
    unsigned int mis = *UART_MIS;

    if (mis & (UART_INT_RX | UART_INT_RT)) {
        while (!(*UART_FR & UART_FR_RXFE)) {
            char c = (char)(*UART_DR & 0xFF);
            unsigned int next_head =
                (uart_rx_head + 1) & (UART_RX_BUFFER_SIZE - 1);

            if (next_head != uart_rx_tail) {
                uart_rx_buffer[uart_rx_head] = c;
                uart_rx_head = next_head;
            }
        }
        *UART_ICR = UART_INT_RX | UART_INT_RT;
    }

    if (mis & UART_INT_ERR) {
        *UART_ICR = UART_INT_ERR;
    }
}