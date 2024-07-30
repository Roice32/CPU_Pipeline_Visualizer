.boot
    jmp main

.code
main:
    mov r0, [0x2000] ; miss
    mov [0x2000], 2 ; hit
    mov r1, [0x2000] ; hit
    add r0, [0x2000] ; hit
    sub [0x3000], 1 ; miss
    end_sim

.data