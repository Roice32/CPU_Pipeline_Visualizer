.boot
    jmp main

.code
main:
    mov r1, loop
    add r1, 2

loop:
    add r0, 1
    cmp r0, 65535
    je end
    add [r1], [r1]
    jmp loop

end:
    push [r1]
    end_sim

.data