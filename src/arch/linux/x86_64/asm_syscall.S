    .text
    .global __linux_x8664_asm_syscall6
    .global __linux_x8664_asm_syscall5
    .global __linux_x8664_asm_syscall4
    .global __linux_x8664_asm_syscall3
    .global __linux_x8664_asm_syscall2
    .global __linux_x8664_asm_syscall1
    .global __linux_x8664_asm_syscall0

    .type __linux_x8664_asm_syscall6, @function
    .type __linux_x8664_asm_syscall5, @function
    .type __linux_x8664_asm_syscall4, @function
    .type __linux_x8664_asm_syscall3, @function
    .type __linux_x8664_asm_syscall2, @function
    .type __linux_x8664_asm_syscall1, @function
    .type __linux_x8664_asm_syscall0, @function

__linux_x8664_asm_syscall6:
    // @param rdi: syscall num
    // @param rsi: arg1
    // @param rdx: arg2
    // @param rcx: arg3
    // @param r8 : arg4
    // @param r9 : arg5
    // @param rsp: arg6
    movq    %rdi, %rax
    movq    %rsi, %rdi
    movq    %rdx, %rsi
    movq    %rcx, %rdx
    movq    %r8,  %r10
    movq    %r9,  %r8
    movq    16(%rsp), %r9
    syscall
    ret

__linux_x8664_asm_syscall5:
    movq    %rdi, %rax
    movq    %rsi, %rdi
    movq    %rdx, %rsi
    movq    %rcx, %rdx
    movq    %r8,  %r10
    movq    %r9,  %r8
    syscall
    ret

__linux_x8664_asm_syscall4:
    movq    %rdi, %rax
    movq    %rsi, %rdi
    movq    %rdx, %rsi
    movq    %rcx, %rdx
    movq    %r8,  %r10
    syscall
    ret

__linux_x8664_asm_syscall3:
    movq    %rdi, %rax
    movq    %rsi, %rdi
    movq    %rdx, %rsi
    movq    %rcx, %rdx
    syscall
    ret

__linux_x8664_asm_syscall2:
    movq    %rdi, %rax
    movq    %rsi, %rdi
    movq    %rdx, %rsi
    syscall
    ret

__linux_x8664_asm_syscall1:
    movq    %rdi, %rax
    movq    %rsi, %rdi
    syscall
    ret

__linux_x8664_asm_syscall0:
    movq    %rdi, %rax
    syscall
    ret
