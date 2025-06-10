.boot
  jmp main ; this jumps to a label in code
.code
main: ; the JMP instruction above should take us here
  mov r0, 0
  mov r1, 50
loop: 
  add r0, 1
  sub r1, 2
  cmp r0, r1
  jl loop

; this will move the value 0xC001 to the “status” location in memory
  mov [status], 0xC001
  end_sim

.data ; this is where we can define data to be used by the program
status:
  dw 0x0000 ; this defines a 16bit word in memory with the value 0