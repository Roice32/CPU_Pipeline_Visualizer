.boot
    jmp main

.code
main:
    mov r0, 3
    mov r1, r0
    mov r0, [someVar]
    mov [someVar], 0
    mov sp, 4
    mov r0, stack_base
    mov [someVar], stack_size
    mov stack_size, 20
    end_sim

.data
someVar:
dw 0x0001