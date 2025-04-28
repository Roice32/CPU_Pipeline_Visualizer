.boot
    jmp main

.code
main:
first:
    jmp second
end:
    end_sim

second:
    mov r0, 3
    jmp third

third:
    cmp 2, 3
    jl end

    jmp end

.data
someVar:
dw 0x0001