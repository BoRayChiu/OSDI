#ifndef MAILBOX_H
#define MAILBOX_H

#define MMIO_BASE       0x3F000000
#define MAILBOX_BASE    (MMIO_BASE + 0xB880)

#define MAILBOX_READ    ((volatile unsigned int*)(MAILBOX_BASE + 0x00))
#define MAILBOX_STATUS  ((volatile unsigned int*)(MAILBOX_BASE + 0x18))
#define MAILBOX_WRITE   ((volatile unsigned int*)(MAILBOX_BASE + 0x20))

#define MAILBOX_EMPTY   0x40000000
#define MAILBOX_FULL    0x80000000

#define GET_BOARD_REVISION  0x00010002
#define GET_VC_MEMORY        0x00010006
#define SET_CLOCK_RATE      0x00038002
#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define END_TAG             0x00000000

extern volatile unsigned int mailbox[36];
int mailbox_call(unsigned char ch);
void print_hardware_info();

#endif