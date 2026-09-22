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
    mov x0, #1
    b exception_common
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
    mov x0, #5
    b exception_common
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
