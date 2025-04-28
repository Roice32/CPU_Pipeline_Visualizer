.boot
    jmp main

.code
main:
    mov r3, sp
    cmp r3, 3000
    jg greater
    
end:
    end_sim

zero:
    cmp 3, 3
    je equal

greater:
    mov r0, 0
    mul r0, 3
    jz zero

equal:
    cmp r0, [someVar]
    je unreached
    jl end

unreached:
    push 100

.data
someVar:
dw 0x0001