#include "uart.h"

typedef unsigned int uint32_t;

#define UART_BASE 0x09000000UL

#define UART_DR (*(volatile uint32_t *)(UART_BASE + 0x00))
#define UART_FR (*(volatile uint32_t *)(UART_BASE + 0x18))

void uart_putc(char c)
{
    while (UART_FR & (1 << 5)) {
    }

    UART_DR = (uint32_t)c;
}

void uart_puts(const char *str)
{
    while (*str) {
        uart_putc(*str++);
    }
}
char uart_getc(void)
{
    while (UART_FR & (1 << 4)) {
    }

    return (char)(UART_DR & 0xFF);
}
