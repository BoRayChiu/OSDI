#include "uart.h"
#include "string.h"
#include "timer.h"
#include "power.h"
#include "mailbox.h"
#include "framebuffer.h"

void exe_cmd(char* cmd) {
    if(strcmp(cmd, "help") == 0) {
        uart_send_string("help: print this help message\r\n");
        uart_send_string("hello: print Hello, World!\r\n");
        uart_send_string("timestamp: print the current timestamp\r\n");
        uart_send_string("reboot: reboot the device\r\n");
        uart_send_string("hardware_info: print hardware information\r\n");
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
    else {
        uart_send_string("Unknown command: ");
        uart_send_string(cmd);
        uart_send_string(". Type 'help' for a list of available commands.\r\n");
    }
}

void main() {
    uart_init();
    uart_send_string("===============\r\n");
    uart_send_string("Welcome to my First OS!\r\n");
    uart_send_string("===============\r\n");

    // Initialize framebuffer and display splash screen
    framebuffer_init();
    show_splash();

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
        else if(c == '\b' || c == 127 && idx > 0) {
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