.boot
  jmp main

.code
main:
  mov [0x2000], 10
  mov [0x2002], 20
  mov [0x2004], 30
  mov [0x2006], 40

  mov z0, [0x2000]
  mov z1, z0

  mov [0x3000], z1
  mov r0, [0x3004]
  end_sim

.data