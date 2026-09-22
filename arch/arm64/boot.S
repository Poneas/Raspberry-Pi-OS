.section ".text.boot"

.global _start

_start:
    ldr x1, =stack_top
    mov sp, x1

    /* Which exception level did firmware/QEMU start us in? */
    mrs x0, CurrentEL
    lsr x0, x0, #2

    cmp x0, #2
    b.eq drop_to_el1

    cmp x0, #1
    b.eq enter_kernel

hang:
    wfe
    b hang


drop_to_el1:
    /*
     * Allow EL1 to access the ARM generic physical counter/timer.
     * CNTHCTL_EL2:
     * bit 0 = EL1PCTEN
     * bit 1 = EL1PCEN
     */
    mrs x0, CNTHCTL_EL2
    orr x0, x0, #3
    msr CNTHCTL_EL2, x0

    msr CNTVOFF_EL2, xzr

    /* EL1 will run as AArch64 */
    mov x0, #0x80000000
    msr HCR_EL2, x0

    /* Give EL1 its stack */
    msr SP_EL1, x1

    /*
     * Return to EL1h.
     * Interrupts initially masked.
     */
    mov x0, #0x3C5
    msr SPSR_EL2, x0

    adr x0, enter_kernel
    msr ELR_EL2, x0

    eret


enter_kernel:
    bl kernel_main

kernel_hang:
    wfe
    b kernel_hang


.section ".bss"

.align 12
stack_bottom:
    .skip 16384
stack_top:
