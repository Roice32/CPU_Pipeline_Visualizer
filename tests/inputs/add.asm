.boot
    jmp main

.code
main:
    add r0, 1
    add r1, [someVar]
    add r2, r1
    add [someVar], r2
    mov r3, someVar
    add [r3], 1
    mov r0, 0
    add r0, 0
    end_sim

.data
someVar:
dw 0x0001