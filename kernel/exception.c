#include "uart.h"
#include "exception.h"

extern char exception_vectors[];

static void uart_puthex(unsigned long value)
{
    const char *hex = "0123456789ABCDEF";

    uart_puts("0x");

    for (int i = 60; i >= 0; i -= 4) {
        uart_putc(hex[(value >> i) & 0xF]);
    }
}

unsigned long exception_current_el(void)
{
    unsigned long current_el;

    __asm__ volatile(
        "mrs %0, CurrentEL"
        : "=r"(current_el)
    );

    return (current_el >> 2) & 0x3;
}

void exception_init(void)
{
    unsigned long el = exception_current_el();
    unsigned long vector_address =
        (unsigned long)exception_vectors;

    if (el == 1) {
        __asm__ volatile(
            "msr VBAR_EL1, %0\n"
            "isb"
            :
            : "r"(vector_address)
            : "memory"
        );
    }
    else if (el == 2) {
        __asm__ volatile(
            "msr VBAR_EL2, %0\n"
            "isb"
            :
            : "r"(vector_address)
            : "memory"
        );
    }
}

void exception_panic(
    unsigned long vector,
    unsigned long esr,
    unsigned long elr,
    unsigned long el)
{
    uart_puts("\r\n\r\n");
    uart_puts("*** KERNEL EXCEPTION ***\r\n");

    uart_puts("Vector: ");
    uart_puthex(vector);

    uart_puts("\r\nCurrent EL: ");
    uart_puthex(el);

    uart_puts("\r\nESR: ");
    uart_puthex(esr);

    uart_puts("\r\nELR: ");
    uart_puthex(elr);

    uart_puts("\r\nSystem halted.\r\n");

    while (1) {
        __asm__ volatile("wfe");
    }
}
