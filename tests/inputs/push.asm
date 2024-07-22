.boot
    jmp main

.code
main:
    mov r0, 3
    push 3
    mov r0, sp
    push [someVar]
    mov r1, sp
    cmp r0, r1

    mov stack_size, 3
    push 0
    mov r0, sp
    end_sim

.data
someVar:
    dw 0x0001