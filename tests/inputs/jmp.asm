.boot

.code
main:
;first:
;    jmp second
;end:
;    end_sim
;
;second:
;    mov r0, 3
;    jmp third
;
;third:
;    cmp 2, 3
;    jl end

    jmp end

end:
    end_sim

.data
someVar:
dw 0x0001