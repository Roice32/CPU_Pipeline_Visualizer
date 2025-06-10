.boot
  jmp main

.code
main:
  mov r0, 0x2000
  mov [counter], 0x0
loop:
  cmp r0, 0x2100
  je end
  cmp [r0], 0x64
  jg loop_step
  add [counter], 1

loop_step:
  add [r0], 0x2
  jmp loop

end:
  push [counter]

.data:
counter:
  dw 0x0000