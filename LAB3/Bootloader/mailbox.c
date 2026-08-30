#include "mailbox.h"
#include "uart.h"
#include "string.h"

volatile unsigned int mailbox[36] __attribute__((aligned(16)));

int mailbox_call(unsigned char ch) {
    unsigned int r = (unsigned long)&mailbox & ~0xF | (ch & 0xF);
    
    while (*MAILBOX_STATUS & MAILBOX_FULL) {
        asm volatile("nop");
    }
    
    *MAILBOX_WRITE = r;
    
    while (1) {
		    while (*MAILBOX_STATUS & MAILBOX_EMPTY) asm volatile("nop");
			if (r == *MAILBOX_READ) return mailbox[1] == REQUEST_SUCCEED;
    }
    
    return 0;
}

void print_hardware_info() {
		/* --- 查詢 Board Revision --- */
		mailbox[0] = 7 * 4; // buffersize
		mailbox[1] = 0; // request or response code
		mailbox[2] = GET_BOARD_REVISION; // tag identifier
		mailbox[3] = 4; // maximum of request and response value buffer's length
		mailbox[4] = 0; // request size
		mailbox[5] = 0; // value buffer
		mailbox[6] = END_TAG;
		
		// we use channel 8
		if (mailbox_call(8)) {
				uart_send_string("Board Revision: 0x");
				uart_send_string(itoa(mailbox[5], 16));
				uart_send_string("\r\n");
		}
		else {
				uart_send_string("Failed to get Board Revision\r\n");
		}
		
		/* --- 查詢 VC Core Memory Base Address --- */
		mailbox[0] = 8 * 4; // buffersize
		mailbox[1] = 0; // request or response code
		mailbox[2] = GET_VC_MEMORY; // tag identifier
		mailbox[3] = 8; // maximum of request and response value buffer's length
		mailbox[4] = 0; // request size
		mailbox[5] = 0; // value buffer
		mailbox[6] = 0; // value buffer
		mailbox[7] = END_TAG;

		// we use channel 8
		if (mailbox_call(8)) {
				uart_send_string("VC Core Memory Base Address: 0x");
				uart_send_string(itoa(mailbox[5], 16));
				uart_send_string("\r\n");
		}
		else {
				uart_send_string("Failed to get VC Core Memory Base Address\r\n");
		}
}