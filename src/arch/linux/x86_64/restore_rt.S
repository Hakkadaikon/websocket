    .section .text
    .align 16
    .global linux_x8664_restore_rt
    .type linux_x8664_restore_rt, @function
linux_x8664_restore_rt:
    .cfi_startproc
    .cfi_signal_frame
    mov $15, %rax
    syscall
    .cfi_endproc
