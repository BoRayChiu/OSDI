#ifndef EXCEPTION_HANDLER_H
#define EXCEPTION_HANDLER_H

#include "uart.h"
#include "string.h"
#include "task.h"
#include "syscall.h"

#define CORE0_IRQ_SOURCE (*(volatile unsigned int *)0x40000060)
#define EC_SVC64 0x15

extern void core_timer_enable(void);
extern void core_timer_handler(void);
extern void local_timer_handler(void);

#endif