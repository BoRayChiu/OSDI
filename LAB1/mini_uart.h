#ifndef MINI_UART_H
#define MINI_UART_H

#define MMIO_BASE 0x3F000000

#define GPFSEL1 (volatile unsigned int*)(MMIO_BASE + 0x00200004)
#define GPPUD (volatile unsigned int*)(MMIO_BASE + 0x00200094)
#define GPPUDCLK0 (volatile unsigned int*)(MMIO_BASE + 0x00200098)

#define AUX_ENABLES (volatile unsigned int*)(MMIO_BASE + 0x00215004)
#define AUX_MU_CNTL_REG (volatile unsigned int*)(MMIO_BASE + 0x00215060)
#define AUX_MU_IER_REG (volatile unsigned int*)(MMIO_BASE + 0x00215044)
#define AUX_MU_LCR_REG (volatile unsigned int*)(MMIO_BASE + 0x0021504C)
#define AUX_MU_MCR_REG (volatile unsigned int*)(MMIO_BASE + 0x00215050)
#define AUX_MU_BAUD_REG (volatile unsigned int*)(MMIO_BASE + 0x00215068)
#define AUX_MU_IIR_REG (volatile unsigned int* )(MMIO_BASE + 0x00215048)
#define AUX_MU_LSR_REG (volatile unsigned int*)(MMIO_BASE + 0x00215054)
#define AUX_MU_IO_REG (volatile unsigned int*)(MMIO_BASE + 0x00215040)

void mini_uart_init();
void mini_uart_send(char c);
char mini_uart_recv();
void mini_uart_send_string(const char* str);

#endif