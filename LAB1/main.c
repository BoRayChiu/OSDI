#include "mini_uart.h"
#include "string.h"
#include "timer.h"
#include "power.h"

void exe_cmd(char* cmd) {
    if(strcmp(cmd, "help") == 0) {
        mini_uart_send_string("help: print this help message\r\n");
        mini_uart_send_string("hello: print Hello, World!\r\n");
        mini_uart_send_string("timestamp: print the current timestamp\r\n");
        mini_uart_send_string("reboot: reboot the device\r\n");
    }
    else if(strcmp(cmd, "hello") == 0) {
        mini_uart_send_string("Hello, World!\r\n");
    }
    else if(strcmp(cmd, "timestamp") == 0) {
        unsigned long timestamp = get_timestamp();
        mini_uart_send_string(itoa(timestamp, 10));
        mini_uart_send_string("\r\n");
    }
    else if(strcmp(cmd, "reboot") == 0) {
        mini_uart_send_string("Rebooting...\r\n");
        reset(100); // Reboot after 100 ticks
        while(1) {}; // Wait for reboot
    }
    else {
        mini_uart_send_string("Unknown command: ");
        mini_uart_send_string(cmd);
        mini_uart_send_string(". Type 'help' for a list of available commands.\r\n");
    }
}

void main() {
    mini_uart_init();
    mini_uart_send_string("===============\r\n");
    mini_uart_send_string("Welcome to my First OS!\r\n");
    mini_uart_send_string("===============\r\n");

    char buffer[100];
    int idx = 0;

    mini_uart_send_string("# ");

    while(1) {
        char c = mini_uart_recv();
        
        if(c == '\r' || c == '\n') {
            mini_uart_send_string("\r\n");
            buffer[idx] = '\0'; // Null-terminate the string
            exe_cmd(buffer);
            idx = 0; // Reset index for the next command
            mini_uart_send_string("# ");
        }
        else if(c == '\b' || c == 127 && idx > 0) {
            idx--;
            mini_uart_send_string("\b \b"); // Handle backspace
        }
        else {
            if (idx < sizeof(buffer) - 1) {
                buffer[idx++] = c;
                mini_uart_send(c);
            }
        }
    }
}