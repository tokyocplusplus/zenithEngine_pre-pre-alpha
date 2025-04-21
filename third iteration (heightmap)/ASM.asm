extern environ

SECTION .data
    text: db 'hi!! this is my opengl project written in x86_64 assembly and C!! enjoy :3 this is the asm print!!', 10
    textLen: equ $-text
    projPath: db './main', 0
    argv: dq projPath, 0

SECTION .text
    global _start

_start:
    ; write(stdout, text, textLen)
    mov rax, 1          ; syscall number: write
    mov rdi, 1          ; describe the operation, STDOUT
    mov rsi, text       ; pointer to message
    mov rdx, textLen    ; length of message
    db 0x0f, 0x05 ; syscall

    ; execve(projPath, argv, environ)
    mov rax, 59         ; syscall number: execve
    mov rdi, projPath   ; filename (char *)
    mov rsi, argv       ; argv (char **)
    mov rdx, [rel environ] ; get actual pointer to environt variables (char **)
    db 0x0f, 0x05       ; syscall

    ; if execve fails, exit(1)
    mov rax, 60         ; syscall number: exit
    mov rdi, 1          ; exit code = 0 (no error/null)
    db 0x0f, 0x05       ; actual memory manip for syscall i think