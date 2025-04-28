.boot
    jmp main

.code
main:
    mov [0x0000], 1
    mov [0x0100], 2
    mov [0x0200], 3
    end_sim

.data