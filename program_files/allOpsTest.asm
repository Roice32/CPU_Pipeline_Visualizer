.boot
    jmp main ; this jumps to a label in code

.code
main:
    add r0, 2
    add r0, 4
    sub r0, 1
    mul r0, 2
    div r1, 3
    mul r5, 4
    end_sim

.data