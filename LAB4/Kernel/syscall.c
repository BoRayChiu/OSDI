#include "syscall.h"
#include "uart.h"
#include "task.h"

unsigned long sys_uart_write(const char *buf, unsigned long size) {
    for (unsigned long i = 0; i < size; i++) {
        uart_send(buf[i]);
    }
    return size;
}

unsigned long sys_uart_read(char *buf, unsigned long size) {
    unsigned long count = 0;
    while (count < size) {
        disable_irq_el1();
        if (uart_rx_available()) {
            buf[count++] = uart_recv_buffered();
            enable_irq_el1();
            continue;
        }
        block_current_on_uart();
        enable_irq_el1();
        schedule();
    }
    return count;
}

void sys_long_kernel_test() {
    uart_send_string("[SYS] Enter Long Syscall\r\n");
    for (volatile unsigned long i = 0; i < 10000000000UL; i++) {
        asm volatile("nop");
    }
    uart_send_string("[SYS] Leave Long Syscall\r\n");
}