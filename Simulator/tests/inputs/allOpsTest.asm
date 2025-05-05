.boot
  jmp main

.code
main:

gauss_sum_loop:
  cmp [param_1], 0
  je gauss_sum_loop_end
  add r0, [param_1]
  sub [param_1], 1
  jmp gauss_sum_loop

gauss_sum_loop_end:
  push r0
  mul r0, 2
  mov [param2], r1
  call count_1_bits
  pop r0
  cmp r0, [result]
  end_sim

count_1_bits:
  mov r2, 0

count_1_bits_loop:
  cmp [param2], 0
  jz count_1_bits_end
  div [param2], 2
  cmp r1, 0
  jg count_1_bits_increment
  mov [param2], r0
  jmp count_1_bits_loop

count_1_bits_increment:
  add r2, 1
  mov [param2], r0
  jmp count_1_bits_loop

count_1_bits_end:
  mov [result], r2
  ret

.data
param_1:
dw 0x0005
param2:
dw 0x0000
result:
dw 0x0000