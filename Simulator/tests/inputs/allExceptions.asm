.boot
  jmp main

.code
main:
  div r0, 0
  mov sp, r1 ; line 17 : 0c65 = mov sp, stack_size 
  mov r0, [775]
  ret
  call 555

div_zero_handler:
  mov [exceptions_count], 1
  add [0x0010], 4
  excp_exit

invalid_decode_handler:
  mov [exceptions_count], 2
  add [0x0010], 2
  excp_exit

misaligned_access_handler:
  mov [exceptions_count], 3
  add [0x0010], 4
  excp_exit

stack_overflow_handler:
  mov [exceptions_count], 4
  add [0x0010], 2
  excp_exit

misaligned_ip_handler:
  mov [exceptions_count], 5
  div r4, 0

.data
exceptions_count:
dw 0x0000

.vector_0
dw div_zero_handler

.vector_2
dw invalid_decode_handler

.vector_4
dw misaligned_access_handler

.vector_6
dw stack_overflow_handler

.vector_8
dw misaligned_ip_handler