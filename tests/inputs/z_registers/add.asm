.boot
    jmp main

.code
main:
    mov [0x2000], 10
    mov [0x2002], 20
    mov [0x2004], 30
    mov [0x2006], 40
    mov z0, [0x2000]

    mov [0x2000], 90
    mov [0x2002], 65516
    mov [0x2004], 0
    mov [0x2006], 404
    mov z1, [0x2000]

    add [0x3000], z0
    add z0, z1
    mov r1, 0x3000
    add z3, [r1]
    end_sim

.data