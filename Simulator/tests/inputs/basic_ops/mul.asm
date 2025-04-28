.boot
    jmp main

.code
main:
    mov r0, 1
    mov r1, 0
    mul r0, r1
    mul [someVar], 30
    mov [someVar], r1
    mul r1, [r4]
    end_sim

.data
someVar:
dw 0x0001