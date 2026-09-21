#include "uart.h"
#include "string.h"
#include "timer.h"
#include "power.h"
#include "mailbox.h"
#include "framebuffer.h"
#include "task.h"
#include "syscall.h"

extern void local_timer_enable(void);
extern void enable_irq_el1(void);
extern unsigned long get_current_el(void);
extern void switch_to_el0(void);
extern void core_timer_enable(void);

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

void delay(unsigned long count) {
    for (unsigned long i = 0; i < count; i++) {
        asm volatile("nop");
    }
}

void foo() {
    int tmp = 5;
    char msg1[] = "Task ";
    uart_write(msg1, sizeof(msg1) - 1);
    char *task_id = itoa(get_pid(), 10);
    uart_write(task_id, strlen(task_id));
    char msg2[] = " after exec, tmp value ";
    uart_write(msg2, sizeof(msg2) - 1);
    char *tmp_str = itoa(tmp, 10);
    uart_write(tmp_str, strlen(tmp_str));
    char msg4[] = "\r\n";
    uart_write(msg4, sizeof(msg4) - 1);
    exit(0);
}

void idle() {
    while(1) {
        schedule();
        delay(1000000);
    }
    uart_send_string("Test finished\r\n");
    while(1);
}

void user_program() {
    int cnt = 1;
    if (fork() == 0) {
        fork();
        delay(100000);
        fork();
        while(cnt < 10) {
            char msg1[] = "Task id: ";
            uart_write(msg1, sizeof(msg1) - 1);
            char *task_id = itoa(get_pid(), 10);
            uart_write(task_id, strlen(task_id));
            char msg2[] = ", cnt: ";
            uart_write(msg2, sizeof(msg2) - 1);
            char *cnt_str = itoa(cnt, 10);
            uart_write(cnt_str, strlen(cnt_str));
            char msg3[] = "\r\n";
            uart_write(msg3, sizeof(msg3) - 1);
            delay(100000);
            cnt++;
        }
        exit(0);
        char msg4[] = "Should not be printed\r\n";
        uart_write(msg4, sizeof(msg4) - 1);
    }
    else {
        char msg1[] = "Task ";
        uart_write(msg1, sizeof(msg1) - 1);
        char *task_id = itoa(get_pid(), 10);
        uart_write(task_id, strlen(task_id));
        char msg2[] = " before exec, cnt value ";
        uart_write(msg2, sizeof(msg2) - 1);
        char *cnt_str = itoa(cnt, 10);
        uart_write(cnt_str, strlen(cnt_str));
        char msg4[] = "\r\n";
        uart_write(msg4, sizeof(msg4) - 1);
        exec(foo);
    }
}

void user_test() {
    do_exec(user_program);
}

static int victim_pid;
void victim() {
    char msg[] = "Victim is running\r\n";
    uart_write(msg, sizeof(msg) - 1);
    while (1) {
        asm volatile("nop");
    }
}

void killer() {
    delay(5000000);
    char msg[] = "Send SIGKILL to victim\r\n";
    uart_write(msg, sizeof(msg) - 1);
    int ret = kill(victim_pid, SIGKILL);
    if (ret == 0) {
        char res[] = "kill() success\r\n";
        uart_write(res, sizeof(res) - 1);
    }
    while (1) {
        asm volatile("nop");
    }
}

void victim_test() {
    do_exec(victim);
}

void killer_test() {
    do_exec(killer);
}

void a_task() {
    while (1) {
        for (int i = 0; i < 5; i++) {
            uart_send_string("A\r\n");
            delay(10000000);
            schedule();
        }
        do_exit(1);
    }
}

void b_task() {
    while (1) {
        for (int i = 0; i < 5; i++) {
            uart_send_string("B\r\n");
            delay(10000000);
            schedule();
        }
        do_exit(2);
    }
}

void c_task() {
    while (1) {
        uart_send_string("C\r\n");
        delay(10000000);
        schedule();
    }
}

void reader_a() {
    char c;
    char msg[] = "Reader A waiting...\r\n";
    uart_write(msg, sizeof(msg) - 1);
    uart_read(&c, 1);
    char got[] = "Reader A got: ";
    uart_write(got, sizeof(got) - 1);
    uart_write(&c, 1);
    uart_write("\r\n", 2);
    while(1) {
        asm volatile("nop");
    }
}

void worker() {
    while(1) {
        char msg[] = "Worker still running\r\n";
        uart_write(msg, sizeof(msg) - 1);
        delay(50000000);
    }
}

void reader_a_test() {
    do_exec(reader_a);
}

void reader_worker() {
    do_exec(worker);
}

void main() {
    uart_init();
    uart_send_string("===============\r\n");
    uart_send_string("Welcome to my First OS!\r\n");
    uart_send_string("Kernel is running at EL1\r\n");
    uart_send_string("===============\r\n");
    
    uart_irq_init();
    //enable_irq_el1();

    // Initialize framebuffer and display splash screen
    framebuffer_init();
    show_splash();

    //uart_send_string("Switching shell to EL0...\r\n");
    //switch_to_el0();

    task_init();

    privilege_task_create(reader_a_test);
    privilege_task_create(reader_worker);

    core_timer_enable();
    enable_irq_el1();

    idle();

    // Running at EL1
    shell();
}