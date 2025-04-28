.boot
    jmp main

.code
main:
    mov [0x2000], 10
    mov [0x2002], 20
    mov [0x2004], 30
    mov [0x2006], 40
    mov z2, [0x2000]

    mov [0x2002], 10101
    mov [0x2004], 65535
    mov [0x2006], 0
    mov z3, [0x2000]

    mul z2, z3
    mul [0x2000], z2
    mul [0x2000], [0x2006] ; to make sure i didn't break regular mul
    end_sim

.data