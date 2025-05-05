.boot
  jmp main

.code
main:
  mov r0, 3
  push 3
  mov r0, sp
  push [someVar]
  mov r1, sp
  cmp r0, r1

  mov stack_base, 200
  push 0
  mov r0, sp
  pop r1
  pop r2
  end_sim

.data
someVar:
  dw 0x0001