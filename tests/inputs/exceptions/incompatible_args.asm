.boot
    jmp main

.code
main:
    mov [0x2000], 0

first:
    add r0, 2 ; line 11 0503 0000 0000 -> add r0, sp
second:
    sub 1000, 22 ; line 15 0821 03e8 00016 -> sub 1000, 22
third:
    mov stack_size, 2 ; line 19 0ca4 0000 0000 -> mov stack_size, stack_base
final:
    end_sim

invalid_decode_handler:
    add [0x2000], 1
    add [0x0010], 6
    excp_exit

.data

.vector_2
dw invalid_decode_handler