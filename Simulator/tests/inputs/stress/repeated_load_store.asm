.boot
  jmp loop

.code 
loop:
  cmp [counter], [iter_count]
  je end
  add [var_a], 1
  add [var_b], [var_a]
  add [counter], 1
  jmp loop

end:
  push [var_a]
  push [var_b]
  end_sim

.data
var_a:
dw 0x0000
var_b:
dw 0x0000
counter:
dw 0x0000
iter_count:
dw 0x0064