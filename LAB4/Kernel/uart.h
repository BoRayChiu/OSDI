#ifndef UART_H
#define UART_H

#define MMIO_BASE 0x3F000000

#define GPFSEL1   (volatile unsigned int*)(MMIO_BASE + 0x00200004)
#define GPPUD     (volatile unsigned int*)(MMIO_BASE + 0x00200094)
#define GPPUDCLK0 (volatile unsigned int*)(MMIO_BASE + 0x00200098)

#define UART_CR   (volatile unsigned int*)(MMIO_BASE + 0x00201030)
#define UART_ICR  (volatile unsigned int*)(MMIO_BASE + 0x00201044)
#define UART_IBRD (volatile unsigned int*)(MMIO_BASE + 0x00201024)
#define UART_FBRD (volatile unsigned int*)(MMIO_BASE + 0x00201028)
#define UART_LCRH (volatile unsigned int*)(MMIO_BASE + 0x0020102C)
#define UART_FR   (volatile unsigned int*)(MMIO_BASE + 0x00201018)
#define UART_DR   (volatile unsigned int*)(MMIO_BASE + 0x00201000)
#define UART_IFLS (volatile unsigned int*)(MMIO_BASE + 0x00201034)
#define UART_IMSC (volatile unsigned int*)(MMIO_BASE + 0x00201038)
#define UART_RIS  (volatile unsigned int*)(MMIO_BASE + 0x0020103C)
#define UART_MIS  (volatile unsigned int*)(MMIO_BASE + 0x00201040)
#define UART_ICR  (volatile unsigned int*)(MMIO_BASE + 0x00201044)

#define UART_INT_RX (1u << 4)
#define UART_INT_TX (1u << 5)
#define UART_INT_RT (1u << 6)
#define UART_INT_FE (1u << 7)
#define UART_INT_PE   (1u << 8)
#define UART_INT_BE   (1u << 9)
#define UART_INT_OE   (1u << 10)
#define UART_INT_ERR (UART_INT_FE | UART_INT_PE | UART_INT_BE | UART_INT_OE)
#define UART_FR_RXFE  (1u << 4)   // RX FIFO empty

#define IRQ_PENDING_2  (volatile unsigned int*)(MMIO_BASE + 0x0000B208)
#define IRQ_ENABLE_2   (volatile unsigned int*)(MMIO_BASE + 0x0000B214)

#define GPU_INTERRUPTS_ROUTING (volatile unsigned int*)(0x4000000C)
#define CORE0_INTERRUPT_SOURCE (volatile unsigned int*)(0x40000060)

void uart_init();
void uart_send(unsigned int c);
char uart_recv();
void uart_send_string(const char* str);
void uart_irq_init(void);
void uart_irq_handler(void);

#endif