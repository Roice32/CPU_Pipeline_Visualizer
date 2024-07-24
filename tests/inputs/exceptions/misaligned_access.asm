.boot
    jmp main

.code
main:
    mov r1, 1025
    mov r0, [r1] ; 1
    add r0, 3
    ; call 775 ; Misaligned IP's job
    cmp [21], [121] ; 2
    mov sp, 1005
    push 20 ; 3
    pop [r1] ; 4

    mov r0, [exceptions_count]
    end_sim

misaligned_access_handler:
    add [exceptions_count], 1
    add [0x0010], 2 ; move ip 1 word fwd
    excp_exit

botched_decode: ; in case vector_4 moves ip to invalid op
    add [0x0010], 2
    excp_exit

.data
exceptions_count:
dw 0x0000

.vector_2
dw botched_decode
.vector_4
dw misaligned_access_handler