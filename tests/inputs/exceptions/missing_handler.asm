.boot
    jmp main

.code
main:
    div r1, 0
    mov r0, 1
    end_sim

invalid_decode_handler:
    mov [0x0010], 0x1004
    excp_exit

.data

.vector_2
dw invalid_decode_handler