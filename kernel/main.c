#include "uart.h"
#include "shell.h"
void kernel_main(void)
{
    uart_puts("\r\n");
    uart_puts("============================\r\n");
    uart_puts(" Raspberry-Pi-OS Kernel\r\n");
    uart_puts("============================\r\n");
    uart_puts("ARM64 kernel initialized.\r\n");
    shell_run();
    while (1) {
        __asm__ volatile("wfe");
    }
}
