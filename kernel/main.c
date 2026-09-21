#include "uart.h"

void kernel_main(void)
{
    uart_puts("\r\n");
    uart_puts("============================\r\n");
    uart_puts(" Raspberry-Pi-OS Kernel\r\n");
    uart_puts("============================\r\n");
    uart_puts("Type something:\r\n");
    uart_puts("> ");

    while (1) {
        char c = uart_getc();

        uart_putc(c);

        if (c == '\r') {
            uart_puts("\n> ");
        }
    }
}
