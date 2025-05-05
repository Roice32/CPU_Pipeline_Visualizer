.boot
  jmp main

.code
main:
  mov r0, 2
  cmp r0, 3
  mul r0, 2
  cmp r1, 4
  cmp [someVar], r4
  mov r0, sp
  cmp r0, 4096
  mov r1, stack_base
  cmp r1, 0
  end_sim

.data
someVar:
dw 0x0001