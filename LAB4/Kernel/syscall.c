#include "syscall.h"
#include "uart.h"

unsigned long sys_uart_write(const char *buf, unsigned long size) {
    for (unsigned long i = 0; i < size; i++) {
        uart_send(buf[i]);
    }
    return size;
}

unsigned long sys_uart_read(char *buf, unsigned long size) {
    for (unsigned long i = 0; i < size; i++) {
        buf[i] = uart_recv();
    }
    return size;
}