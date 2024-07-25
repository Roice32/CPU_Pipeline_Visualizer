.boot
    jmp main

.code
main:
    mov [excp_count], 0

    mov r0, r1 ; line 12 -> 0d10 mov r0, z0

    cmp r1, r2 ; line 13 -> 1930 cmp r1, z1

    mul r2, r3 ; line 14 -> 126b mul z3, r3

    add z0, z2 ; lines 15-17 -> 0601 0002 0000 add z0, 2

    mov [0x2000], 1
    mov [0x2002], 2
    mov [0x2004], 3
    mov [0x2006], 4
    mov z0, [0x2000]
    mov [0x2004], 0
    mov z1, [0x2000]

    div z0, z1 ; div by zero

    end_sim

invalid_dec_handl:
    add [excp_count], 1
    add [0x0010], 2
    excp_exit

div_zero_handl:
    add [excp_count], 1
    end_sim

.data
excp_count:
dw 0x0000

.vector_0
dw div_zero_handl
.vector_2
dw invalid_dec_handl