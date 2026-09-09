#ifndef SYSCALL_H
#define SYSCALL_H

#define SYS_UART_WRITE 0
#define SYS_UART_READ 1
#define SYS_EXEC 2
#define SYS_FORK 3
#define SYS_EXIT 4
#define SYS_ENABLE_TIMER 5
#define SYS_GET_PID 6

extern unsigned long uart_write(const char *buf, unsigned long size);
extern unsigned long uart_read(char *buf, unsigned long size);
extern int exec(void (*func)(void));
extern int fork(void);
extern void exit(int status);
extern int get_pid(void);

unsigned long sys_uart_write(const char *buf, unsigned long size);
unsigned long sys_uart_read(char *buf, unsigned long size);

#endif