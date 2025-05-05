.boot
  jmp main

.code
main:
  mov [0x2000], addr4
  mov [0x2002], addr1
  mov [0x2004], addr3
  mov [0x2006], addr2

  mov z0, [0x2000]
  gather z1, z0

  gather z2, z0 ; changed to gather z2, r0
  end_sim

invalid_decode_handl:
  push 3
  end_sim

.data

addr1:
dw 0x0010

addr2:
dw 0xffff

addr3:
dw 0x0110

addr4:
dw 0x0000

.vector_2
dw invalid_decode_handl