SECTION .data
    text: db 'hi!! this is my opengl project written in x86_64 assembly and C!! enjoy :3 this is the asm print!!', 10
    textLen: equ $-text

    projPath: db './main', 0

    ; Pointers to arguments (64-bit pointers now)
    argv: dq projPath, 0
    envp: dq 0

SECTION .text
    global _start

_start:
    ; write(stdout, text, textLen)
    mov rax, 1          ; db 0x0F, 0x05 number for write
    mov rdi, 1          ; file descriptor: stdout
    mov rsi, text       ; pointer to text
    mov rdx, textLen    ; length of text
    db 0x0F, 0x05       ; invoke syscall

    ; execve(projPath, argv, envp)
    mov rax, 59         ; db 0x0F, 0x05 number for execve
    mov rdi, projPath   ; pointer to program path
    mov rsi, argv       ; pointer to argv array
    mov rdx, envp       ; pointer to envp array
    db 0x0F, 0x05       ; invoke syscall

    ; if execve fails, exit(1)
    ;mov rax, 60         ; db 0x0F, 0x05 number for exit
    ;xor rdi, rdi        ; status = 0
    ;db 0x0F, 0x05       ; syscall
