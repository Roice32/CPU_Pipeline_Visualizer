.boot
    jmp main

.code
main:
    pop r0 ; 1
    push 200
    push 300
    ret ; 2
    push 400
    mov stack_size, 2
    push 500 ; 3
    mov stack_size, 7
    push 600 ; 4
    call 224 ; 5

    mov r0, [exceptions_count]
    end_sim

stack_overflow_handler:
    add [exceptions_count], 1
    add [0x0010], 2 ; move ip 1 word fwd
    excp_exit

botched_decode: ; in case vector_4 moves ip to (in)valid op with invalid srcs
    add [0x0010], 2
    excp_exit

.data
exceptions_count:
dw 0x0000

.vector_2
dw botched_decode
.vector_6
dw stack_overflow_handler