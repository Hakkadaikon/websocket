    .text
    .global linux_x8664_asm_syscall
    .type linux_x8664_asm_syscall, @function
linux_x8664_asm_syscall:
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

    // Perform the syscall.
    syscall

    ret

