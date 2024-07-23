.boot
    jmp main

.code
main:
    div r1, 0
    end_sim

div_zero_handler:
    mov r0, 1
    end_sim

.data

.vector_0
dw div_zero_handler