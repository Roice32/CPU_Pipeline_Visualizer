.boot
    jmp main

.code
main:
    
first:
    mov r0, 2 
    call method
    mov r0, [0x2000]
    add [someVar], 1
    cmp [someVar], r1
    sub [someVar], 0
    jg first
    jz final
    end_sim

method:
    mov r0, 10
    mul r0, 10
    mov [0x2000], r1
    ret
    
final:
    push 0x0000
    end_sim

.data
someVar:
dw 0xffff