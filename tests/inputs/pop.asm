.boot
    jmp main

.code
main:
    mov r0, 3
    push 4
    push 5
    pop
    pop r0
    end_sim

.data
someVar:
dw 0x0001