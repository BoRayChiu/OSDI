#include "uart.h"

#define LOCAL_TIMER_IRQ_ROUTING (*(volatile unsigned int *)0x40000024)
#define LOCAL_TIMER_CONTROL_REG (*(volatile unsigned int *)0x40000034)
#define LOCAL_TIMER_IRQ_CLR (*(volatile unsigned int *)0x40000038)

extern void local_timer_enable(void);
extern void local_timer_handler(void);
extern void irq_enable_nested(void);
extern void irq_disable_nested(void);

static void long_delay(void) {
    for (volatile unsigned long i = 0; i < 500000000UL; i++) {
        asm volatile("nop");
    }
}

void local_timer_enable(void) {
    // route local timer irq to Core 0
    LOCAL_TIMER_IRQ_ROUTING = 0;
  
    unsigned int reload = 1000000000;
    // bit 29: interrupt enable
    // bit 28: timer enable
    LOCAL_TIMER_CONTROL_REG = (1u << 29) | (1u << 28) | reload;
}

void local_timer_handler(void){
    // Top Half
    // bit 30: reload
    // bit 31: clear interrupt flag
    LOCAL_TIMER_IRQ_CLR = 0xC0000000;
    uart_send_string("\r\n=== LOCAL ENTER ===\r\n");
    uart_send_string("[Local] Top Half\r\n");

    irq_enable_nested();

    // Bottom Half
    uart_send_string("[Local] Bottom Half start\r\n");
    long_delay();
    uart_send_string("[Local] Bottom Half end\r\n");
    uart_send_string("=== LOCAL EXIT ===\r\n\r\n");

    irq_disable_nested();
}