.boot
  jmp main

.code
main:
  mov [0x2000], 10
  mov [0x2002], 20
  mov [0x2004], 30
  mov [0x2006], 40
  mov z0, [0x2000]

  mov [0x2000], 1
  mov [0x2002], 2
  mov [0x2004], 3
  mov [0x2006], 4
  mov z1, [0x2000]

  sub [0x3000], z0
  sub z0, z1
  mov r1, 0x3000
  sub z3, [r1]
  end_sim

.data