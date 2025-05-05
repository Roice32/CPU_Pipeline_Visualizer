.boot
  jmp main

.code
main:
  call fib
  push [glob_var_a]
  end_sim

fib:
  cmp [param_n], 2
  jl base_case_1
  je base_case_2

  push [param_n]
  sub [param_n], 1
  call fib
  push [glob_var_a]
  
  sub [param_n], 1
  call fib
  pop [glob_var_b]
  add [glob_var_a], [glob_var_b]
  pop [param_n]
  ret

base_case_1:
  mov [glob_var_a], 0
  ret

base_case_2:
  mov [glob_var_a], 1
  ret

.data:
param_n:
dw 0x0006
glob_var_a:
dw 0x0000
glob_var_b:
dw 0x0000
