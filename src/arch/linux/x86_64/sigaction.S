    .text
    .global linux_x8664_assembly_sigaction
    .type linux_x8664_assembly_sigaction, @function
linux_x8664_assembly_sigaction:
    // According to the System V AMD64 calling convention, the arguments are passed as follows:
    //   @param rdi: signum
    //   @param rsi: act
    //   @param rdx: oldact
    //   @param rcx: sigsetsize
    // However, for syscalls the 4th argument must be placed in r10.
    // So, move the value from rcx to r10.
    movq    %rcx, %r10

    // Load the syscall number for sys_rt_sigaction into rax.
    // Here we assume __NR_rt_sigaction is 13; replace with the correct value if necessary.
    movq    $13, %rax

    // Perform the syscall.
    syscall

    ret

