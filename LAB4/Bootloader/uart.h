#ifndef UART_H
#define UART_H

#define MMIO_BASE 0x3F000000

#define GPFSEL1 (volatile unsigned int*)(MMIO_BASE + 0x00200004)
#define GPPUD (volatile unsigned int*)(MMIO_BASE + 0x00200094)
#define GPPUDCLK0 (volatile unsigned int*)(MMIO_BASE + 0x00200098)

#define UART_CR (volatile unsigned int*)(MMIO_BASE + 0x00201030)
#define UART_ICR (volatile unsigned int*)(MMIO_BASE + 0x00201044)
#define UART_IBRD (volatile unsigned int*)(MMIO_BASE + 0x00201024)
#define UART_FBRD (volatile unsigned int*)(MMIO_BASE + 0x00201028)
#define UART_LCRH (volatile unsigned int*)(MMIO_BASE + 0x0020102C)
#define UART_FR (volatile unsigned int*)(MMIO_BASE + 0x00201018)
#define UART_DR (volatile unsigned int*)(MMIO_BASE + 0x00201000)

void uart_init();
void uart_send(unsigned int c);
char uart_recv();
void uart_send_string(const char* str);

#endif