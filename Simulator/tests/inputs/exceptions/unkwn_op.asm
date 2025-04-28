.boot
    jmp main

.code
main:
    div r1, 1 ; modify line 9 of hex into 'fd21' or '2121' (or smth)
    mov r0, 1
    add r0, 100
    end_sim

div_zero_handler:
    mov [0x0010], 0x1004
    excp_exit

unkwn_op_handler:
    mov [0x0018], 200
    mov [0x0010], 0x1008
    excp_exit 

.data

.vector_0
dw div_zero_handler
.vector_2
dw unkwn_op_handler
