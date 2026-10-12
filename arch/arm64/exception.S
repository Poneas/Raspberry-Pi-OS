.section ".text.exceptions", "ax"

.align 11
.global exception_vectors
exception_vectors:

/* Current EL with SP0 */
vector_0:
    mov x0, #0
    b exception_common
    .balign 128

vector_1:
    b irq_entry
    .balign 128

vector_2:
    mov x0, #2
    b exception_common
    .balign 128

vector_3:
    mov x0, #3
    b exception_common
    .balign 128

/* Current EL with SPx */
vector_4:
    mov x0, #4
    b exception_common
    .balign 128

vector_5:
    b irq_entry
    .balign 128

vector_6:
    mov x0, #6
    b exception_common
    .balign 128

vector_7:
    mov x0, #7
    b exception_common
    .balign 128

/* Lower EL, AArch64 */
vector_8:
    mov x0, #8
    b exception_common
    .balign 128

vector_9:
    mov x0, #9
    b exception_common
    .balign 128

vector_10:
    mov x0, #10
    b exception_common
    .balign 128

vector_11:
    mov x0, #11
    b exception_common
    .balign 128

/* Lower EL, AArch32 */
vector_12:
    mov x0, #12
    b exception_common
    .balign 128

vector_13:
    mov x0, #13
    b exception_common
    .balign 128

vector_14:
    mov x0, #14
    b exception_common
    .balign 128

vector_15:
    mov x0, #15
    b exception_common
    .balign 128


exception_common:
    /* Determine the exception level we're currently running at */
    mrs x3, CurrentEL
    lsr x3, x3, #2

    cmp x3, #2
    b.eq exception_from_el2

    /* EL1 */
    mrs x1, ESR_EL1
    mrs x2, ELR_EL1
    b exception_panic

exception_from_el2:
    mrs x1, ESR_EL2
    mrs x2, ELR_EL2
    b exception_panic
irq_entry:
    /*
     * IRQ stack frame:
     *
     * 0   - 247 : x0-x30
     * 248 - 255 : padding
     * 256       : ELR_EL1
     * 264       : SPSR_EL1
     *
     * Total: 272 bytes
     */

    sub sp, sp, #272

    stp x0,  x1,  [sp, #0]
    stp x2,  x3,  [sp, #16]
    stp x4,  x5,  [sp, #32]
    stp x6,  x7,  [sp, #48]
    stp x8,  x9,  [sp, #64]
    stp x10, x11, [sp, #80]
    stp x12, x13, [sp, #96]
    stp x14, x15, [sp, #112]
    stp x16, x17, [sp, #128]
    stp x18, x19, [sp, #144]
    stp x20, x21, [sp, #160]
    stp x22, x23, [sp, #176]
    stp x24, x25, [sp, #192]
    stp x26, x27, [sp, #208]
    stp x28, x29, [sp, #224]

    str x30, [sp, #240]

    /*
     * Save exception return state.
     */
    mrs x0, ELR_EL1
    str x0, [sp, #256]

    mrs x0, SPSR_EL1
    str x0, [sp, #264]

    /*
     * Pass current IRQ frame to C.
     *
     * irq_handler() returns the stack frame
     * that should run next.
     */
    mov x0, sp
    bl irq_handler

    /*
     * x0 may point to another task's stack.
     */
    mov sp, x0

    /*
     * Restore selected task's exception state.
     */
    ldr x1, [sp, #256]
    msr ELR_EL1, x1

    ldr x1, [sp, #264]
    msr SPSR_EL1, x1

    ldp x0,  x1,  [sp, #0]
    ldp x2,  x3,  [sp, #16]
    ldp x4,  x5,  [sp, #32]
    ldp x6,  x7,  [sp, #48]
    ldp x8,  x9,  [sp, #64]
    ldp x10, x11, [sp, #80]
    ldp x12, x13, [sp, #96]
    ldp x14, x15, [sp, #112]
    ldp x16, x17, [sp, #128]
    ldp x18, x19, [sp, #144]
    ldp x20, x21, [sp, #160]
    ldp x22, x23, [sp, #176]
    ldp x24, x25, [sp, #192]
    ldp x26, x27, [sp, #208]
    ldp x28, x29, [sp, #224]

    ldr x30, [sp, #240]

    add sp, sp, #272

    eret
