.boot
  jmp main

.code
main:
  mov r0, 1000
loop:
  cmp r0, 1256
  je end
  mov [r0], r0
  add r0, 2
  jmp loop

end:
  end_sim

.data