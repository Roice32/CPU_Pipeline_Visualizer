.boot
  jmp main

.code
main:
  mov r0, [someVar]
  add r1, r2 ; will become end_sim

  add [someVar], 1

  mov r3, main
  add r3, 4
  mov [r3], 15360
  add r3, 2
  mov [r3], 0

  jmp main

.data
someVar:
dw 0x0001