.section ".text"

.global uart_write
.global uart_read
.global exec
.global fork
.global exit
.global get_pid

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

fork:
  mov x8, #3 // SYS_FORK
  svc #0
  ret

exit:
  mov x8, #4 // SYS_EXIT
  svc #0
  ret

get_pid:
 mov x8, #6 // SYS_GET_PID
 svc #0
 ret
