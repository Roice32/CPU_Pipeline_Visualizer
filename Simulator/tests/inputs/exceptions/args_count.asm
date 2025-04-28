.boot
    jmp main

.code
main:
    mov [0x2000], 0

first:
    add r0, 2 ; line 11: 0500 -> 2 becomes NULL_VAL
second:
    sub r1, 2 ; line 14: 0801 -> r1 becomes NULL_VAL
third:
    call [0x0000] ; line 17: 3400 -> src1 becomes NULL_VAL
fourth:
    end_sim ; line 20: 3c01 -> src2 becomes IMM
final:
    end_sim
    end_sim
    end_sim ; final excp_exit will skip somewhere around here.

invalid_decode_handler:
    add [0x2000], 1
    add [0x0010], 6
    excp_exit

.data

.vector_2
dw invalid_decode_handler