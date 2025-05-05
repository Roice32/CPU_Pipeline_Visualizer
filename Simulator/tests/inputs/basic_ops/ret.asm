.boot
  jmp main

.code
main:
  ;mov stack_size, 41
  ;ret
rec:
  ;call rec
  ;ret
  ;end_sim

  mov [someVar], 3
loop:
  call dec
  cmp [someVar], 0
  jg loop
  end_sim

dec:
  sub [someVar], 1
  ret

.data
someVar:
dw 0x0001