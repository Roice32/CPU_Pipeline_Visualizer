.boot
  jmp main

.code
main:
  mov [someVar], 2
someCode:
  call methodA
  ;cmp [someVar], 0
  ;jz end
  ;jmp someCode
end:
  end_sim

methodA:
  mov r1, 1
  sub [someVar], r1
  call methodB
  ret

methodB:
  mov [someVar], 0
  ret

.data
someVar:
dw 0x0001