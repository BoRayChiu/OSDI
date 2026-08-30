#include "uart.h"

void main() {
    uart_init();
    uart_send_string("Bootloader is ready! Waiting for kernel...\n");

    // 1. Read size of Kernel
    unsigned int kernel_size = 0;
    kernel_size |= uart_recv() << 0;
    kernel_size |= uart_recv() << 8;
    kernel_size |= uart_recv() << 16;
    kernel_size |= uart_recv() << 24;

    uart_send_string("Kernel size received! Send data now...\n");

    // 2. Read Kernel data
    unsigned char* kernel_data = (unsigned char*)0x80000;
    for (unsigned int i = 0; i < kernel_size; i++) {
        kernel_data[i] = uart_recv();
    }
    uart_send_string("Kernel data received! Jumping to kernel...\n");

    // 3. Jump to Kernel
    void (*kernel_entry)() = (void (*)())0x80000;
    kernel_entry();
}