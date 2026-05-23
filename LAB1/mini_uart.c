#include "mini_uart.h"

void wait_cycles(int cycles) {
    for (int i = 0; i < cycles; i++) {
        asm volatile("nop");
    }
}

void mini_uart_init() {
    *GPFSEL1 &= ~(7 << 12); // Clear GPIO14
    *GPFSEL1 |= 2 << 12; // Set GPIO14 to ALT5
    *GPFSEL1 &= ~(7 << 15); // Clear GPIO15
    *GPFSEL1 |= 2 << 15; // Set GPIO15 to ALT5
    *GPPUD = 0; // Disable pull-up/down
    wait_cycles(150); // wait 150 cycles
    *GPPUDCLK0 = 3 << 14; // Apply to GPIO14 and GPIO15
    wait_cycles(150); // wait 150 cycles
    *GPPUD = 0; // Flush GPIO setup
    *GPPUDCLK0 = 0; // Flush GPIO setup


    *AUX_ENABLES |= 1; // Enable mini uart
    *AUX_MU_CNTL_REG = 0; // Disable transmitter and receiver during configuration
    *AUX_MU_IER_REG = 0; // Disable interrupt
    *AUX_MU_LCR_REG = 3; // Set the data size to 8 bit
    *AUX_MU_MCR_REG = 0; // Don’t need auto flow control
    *AUX_MU_BAUD_REG = 270; // Set baud rate to 115200
    *AUX_MU_IIR_REG = 6; // No FIFO
    *AUX_MU_CNTL_REG = 3; // Enable transmitter and receiver
}

void mini_uart_send(char c) {
    while(!(*AUX_MU_LSR_REG & 32)) {
        // Wait until the transmitter is empty
    }
    *AUX_MU_IO_REG = c; // Write the character to the transmit FIFO
}

char mini_uart_recv() {
    while(!(*AUX_MU_LSR_REG & 1)) {
        // Wait until there is data to read
    }
    return (char)(*AUX_MU_IO_REG); // Read the character from the receive FIFO
}

void mini_uart_send_string(const char* str) {
    while (*str) {
        mini_uart_send(*str++);
    }
}