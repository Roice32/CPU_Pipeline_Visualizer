.boot
  jmp main

.code
main:
  mov r1, firstLabel
  sub r1, 1
  jmp r1; 1st exception

firstLabel:
  mov r1, 331
  call r1 ; 2

misaligned_ip_handler:
  add [exceptions_count], 1
  cmp [exceptions_count], 2
  je end
  add [0x0010], 1 ; move ip 1 byte fwd
  excp_exit

end:
  end_sim

botched_decode: ; in case vector_4 moves ip to (in)valid op with invalid srcs
  add [0x0010], 2
  excp_exit

.data
exceptions_count:
dw 0x0000

.vector_2
dw botched_decode
.vector_8
dw misaligned_ip_handler