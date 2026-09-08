.section ".text"

.global uart_write
.global uart_read
.global exec

uart_write:
  mov x8, #0 // SYS_UART_WRITE
  svc #0
  ret

uart_read:
  mov x8, #1 // SYS_UART_READ
  svc #0
  ret

exec:
  mov x8, #2 // SYS_EXEC
  svc #0
  ret
