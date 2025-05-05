.boot
  jmp main

.code
main:
  mov r1, 1
  sub r1, 1
  sub r2, [someVar]
  mov r0, someVar
  sub r0, 0
  sub [r0], 1
  end_sim

.data
someVar:
dw 0x0001