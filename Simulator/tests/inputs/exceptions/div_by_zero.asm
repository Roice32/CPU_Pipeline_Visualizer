.boot
  jmp main

.code
main:
  div r1, 0
  mov r0, 1
  end_sim

div_zero_handler:
  mov [0x0010], 0x1004
  excp_exit

.data

.vector_0
dw div_zero_handler