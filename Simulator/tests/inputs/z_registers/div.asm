.boot
    jmp main

.code
main:
    mov [0x2000], 10
    mov [0x2002], 20
    mov [0x2004], 30
    mov [0x2006], 40
    mov z2, [0x2000]

    mov [0x2002], 3
    mov [0x2004], 65535
    mov [0x2006], 1
    mov z3, [0x2000]

    div z2, z3
    div [0x2000], z2
    div [0x2000], 3 ; to make sure i didn't break regular div

    mov [0x2000], 0
    mov z2, [0x2000]
    div z3, z2
    end_sim

div_by_zero_handler:
    push 0
    end_sim

.data

.vector_0
dw div_by_zero_handler