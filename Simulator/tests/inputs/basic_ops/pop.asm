.boot
  jmp main

.code
main:
  mov r1, 3
  push r1
  push 5
  pop
  pop r0
  end_sim

.data
someVar:
dw 0x0001