.model small, c

.data

    public csip
    public outpsp

    envptr      dw 0
    params      dd 0
    fcb1ptr     dd 0
    fcb2ptr     dd 0
    sssp        dd 0
    csip        dd 0

    cmdline     db 0
    fcb1        db 38 dup (0)
    fcb2        db 38 dup (0)
    outpsp      dw 0

.code

    jmpptr  dd 0

    myds    dw 0
    otherds dw 0

public myds

public load_program
load_program proc

    push bp
    mov bp, sp

    mov word ptr [params], offset cmdline
    mov word ptr [params+2], ds

    mov word ptr [fcb1ptr], offset fcb1
    mov word ptr [fcb1ptr+2], ds

    mov word ptr [fcb2ptr], offset fcb2
    mov word ptr [fcb2ptr+2], ds

    mov ax, 4b01h
    mov dx, offset [bp+4]
    mov bx, ds
    mov es, bx
    mov bx, offset envptr
    int 21h ; DOS: Load But Do Not Execute Program
    jc error

    mov ah, 62h ; DOS: Get PSP
    int 21h

    mov outpsp, bx

    mov ax, 0
    pop bp
    ret

error:
    mov ax, 1
    pop bp
    ret

load_program endp


public start_program
start_program proc

    mov ax, word ptr csip+2
    mov bx, word ptr csip

    mov word ptr jmpptr+2, ax
    mov word ptr jmpptr, bx

    mov ss, word ptr sssp+2
    mov sp, word ptr sssp

    mov ds, outpsp
    mov es, outpsp

    pop ax

    jmp dword ptr jmpptr

start_program endp


public ds_trampoline_init
ds_trampoline_init proc

    push ax
    mov ax, ds
    mov myds, ax
    pop ax
    ret

ds_trampoline_init endp

public ds_trampoline_start
ds_trampoline_start proc

    push ax
    mov ax, ds
    mov otherds, ax
    mov ax, myds
    mov ds, ax
    pop ax
    ret

ds_trampoline_start endp


public ds_trampoline_end
ds_trampoline_end proc

    push ax
    mov ax, otherds
    mov ds, ax
    pop ax
    ret

ds_trampoline_end endp

test1 dw 0

public t1
t1 proc

    push bp
    mov bp, sp
    mov bx, [bp + 4]
    mov test1, bx
    pop bp
    ret

t1 endp


public t2
t2 proc

    mov ax, cs
    ret

t2 endp

end
