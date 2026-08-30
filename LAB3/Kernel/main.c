#include "uart.h"
#include "string.h"
#include "timer.h"
#include "power.h"
#include "mailbox.h"
#include "framebuffer.h"

extern void local_timer_enable(void);
extern void enable_irq_el1(void);
extern unsigned long get_current_el(void);
extern void switch_to_el0(void);

void syscall_core_timer_enable() {
    asm volatile("svc #2");
}

void exe_cmd(char* cmd) {
    if(strcmp(cmd, "help") == 0) {
        uart_send_string("help: print this help message\r\n");
        uart_send_string("hello: print Hello, World!\r\n");
        uart_send_string("timestamp: print the current timestamp\r\n");
        uart_send_string("reboot: reboot the device\r\n");
        uart_send_string("hardware_info: print hardware information\r\n");
        uart_send_string("exc: trigger an exception\r\n");
        uart_send_string("irq: enable core timer IRQ\r\n");
        uart_send_string("local_irq: enable local timer IRQ\r\n");
        uart_send_string("el: print current exception level, can't use because it's EL0 now\r\n");
    }
    else if(strcmp(cmd, "hello") == 0) {
        uart_send_string("Hello, World!\r\n");
    }
    else if(strcmp(cmd, "timestamp") == 0) {
        unsigned long timestamp = get_timestamp();
        uart_send_string(itoa(timestamp, 10));
        uart_send_string("\r\n");
    }
    else if(strcmp(cmd, "reboot") == 0) {
        uart_send_string("Rebooting...\r\n");
        reset(100); // Reboot after 100 ticks
        while(1) {}; // Wait for reboot
    }
    else if(strcmp(cmd, "hardware_info") == 0) {
        print_hardware_info();
    }
    else if(strcmp(cmd, "exc") == 0) {
        asm volatile("svc #1");
    }
    else if(strcmp(cmd, "irq") == 0) {
        uart_send_string("Enable core timer IRQ\r\n");
        syscall_core_timer_enable();
    }
    else if(strcmp(cmd, "local_irq") == 0) {
        uart_send_string("Enable local timer IRQ\r\n");
        local_timer_enable();
    }
    else if(strcmp(cmd, "el") == 0) {
        uart_send_string("Now running at EL");
        char el = '0' + get_current_el();
        uart_send(el);
        uart_send_string("\r\n");
    }
    else {
        uart_send_string("Unknown command: ");
        uart_send_string(cmd);
        uart_send_string(". Type 'help' for a list of available commands.\r\n");
    }
}

void shell() {
    char buffer[100];
    int idx = 0;

    uart_send_string("# ");
    while(1) {
        char c = uart_recv();
        
        if(c == '\r' || c == '\n') {
            uart_send_string("\r\n");
            buffer[idx] = '\0'; // Null-terminate the string
            exe_cmd(buffer);
            idx = 0; // Reset index for the next command
            uart_send_string("# ");
        }
        else if((c == '\b' || c == 127) && idx > 0) {
            idx--;
            uart_send_string("\b \b"); // Handle backspace
        }
        else {
            if (idx < sizeof(buffer) - 1) {
                buffer[idx++] = c;
                uart_send(c);
            }
        }
    }
}

void main() {
    uart_init();
    uart_send_string("===============\r\n");
    uart_send_string("Welcome to my First OS!\r\n");
    uart_send_string("Kernel is running at EL1\r\n");
    uart_send_string("===============\r\n");
    
    //uart_irq_init();
    //enable_irq_el1();

    // Initialize framebuffer and display splash screen
    framebuffer_init();
    show_splash();

    uart_send_string("Switching shell to EL0...\r\n");
    switch_to_el0();
}