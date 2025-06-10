.boot

  jmp main

.code

main:
  mov r0, [var_1] ; r0 = 1
  mov r1, 0x2     ; r1 = 2
  add r0, r1      ; r0 = 3
  sub r0, 0x1     ; r0 = 2
  push r0         ; push r0 (2) onto stack
  cmp r0, 0x0     ; flags.Z = flags.E = 0, flags.G = 1
  jg post_jg      ; taken
  end_sim         ; skipped

post_jg:
  mov r1, var_1     ; r1 = address of var_1
  add [r1], [var_2] ; var_1 = 4
  push [r1]         ; push var_1 (4) onto stack
  cmp [var_1], 0x3  ; flags.Z = flags.G = 0, flags.E = 1
  je post_je        ; taken
  end_sim           ; skipped

post_je:
  pop         ; pop 4 from stack, don't save it anywhere
  pop r2      ; pop 2 from stack into r2
  mov r3, 0xf ; r3 = 15
  mul r2, r3  ; r0 = 0, r1 = 30
  cmp 0x0, r1 ; flags.Z = flags.E = flags.G = 0
  jl post_jl  ; taken
  end_sim     ; skipped

post_jl:
  div 0x0021, [var_2] ; r0 = 33 / 2 = 16, r1 = 33 % 2 = 1
  div r0, 0x0         ; division by zero, triggers exception
  call simd_exec      ; call subroutine that executes SIMD instructions
  end_sim             ; simulation ends here

simd_exec:
  jmp setup_loop       ; jump to setup loop
simd_exec_post_setup:
  mov z3, [batch_addr] ; z3 = address of batch_addr
  gather z2, z3        ; gather data from batch_addr into z2
  mul z2, [coeffs]     ; multiply each element in z2 by 15
  scatter z1, z3       ; scatter lower half of z2 back into batch_addr
  ret                  ; return from subroutine, execution continues after call

setup_loop:
  mov [counter], 0x0 ; initialize counter to 0
loop_step:
  mov r3, batch_addr ; r3 = address of batch_addr[0]
  add r3, [counter]  ; r3 = address of batch_addr[i]
  mov r2, data_0     ; r2 = address of data_0
  add r2, [counter]  ; r2 = address of data_i
  mov [r3], r2       ; batch_addr[i] = address of data_i

  add [counter], 0x2       ; store result in counter
  cmp [counter], 0x8       ; compare counter with 8
  jl loop_step             ; if counter < 8, continue loop
  jmp simd_exec_post_setup ; jump back to simd_exec after setup

div_zero_handler:
  mov [exceptions_count], 1 ; set exception count to 1
  add [0x0010], 4           ; return ip in save state region += 4 (sizeof "div r0, 0x0")
  excp_exit                 ; exit exception handler, return to ip saved at 0x0010

.data

var_1:
  dw 0x0001

var_2:
  dw 0x0002

counter:
  dw 0x0000

batch_addr:
  dblock 0xffff, 4

data_0:
  dw 0x0002
data_1:
  dw 0x0006
data_2:
  dw 0x003e
data_3:
  dw 0x0afa

coeffs:
  dblock 0x000f, 4

exceptions_count:
  dw 0x0000

.vector_0
  dw div_zero_handler