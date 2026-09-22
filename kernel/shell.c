#include "uart.h"
#include "shell.h"
#include "timer.h"
#define CMD_BUFFER_SIZE 64

static int str_equal(const char *a, const char *b)
{
    while (*a && *b) {
        if (*a != *b)
            return 0;

        a++;
        b++;
    }

    return (*a == '\0' && *b == '\0');
}

static void execute_command(const char *command)
{
    if (str_equal(command, "help")) {
        uart_puts(
            "Available commands:\r\n"
            "  help  - Show available commands\r\n"
            "  info  - Show kernel information\r\n"
            "  clear - Clear the terminal\r\n"
            "  exc   - Trigger test exception\r\n"
            "  uptime - Show system uptime\r\n"
        );
    }
    else if (str_equal(command, "info")) {
        uart_puts(
            "Kernel: Raspberry-Pi-OS\r\n"
            "Architecture: ARM64 (AArch64)\r\n"
            "Platform: QEMU virt\r\n"
        );
    }
    else if (str_equal(command, "clear")) {
        uart_puts("\033[2J\033[H");
    }
    else if (str_equal(command, "exc")) {
    uart_puts("Triggering test exception...\r\n");

    __asm__ volatile("brk #0");
    }
    else if (str_equal(command, "uptime")) {
    unsigned long seconds = timer_get_seconds();

    uart_puts("Uptime: ");

    if (seconds == 0) {
        uart_putc('0');
    }
    else {
        char buffer[21];
        int i = 0;

        while (seconds > 0) {
            buffer[i++] = '0' + (seconds % 10);
            seconds /= 10;
        }

        while (i > 0) {
            uart_putc(buffer[--i]);
        }
    }

    uart_puts(" seconds\r\n");
    }
    else if (command[0] != '\0') {
        uart_puts("Unknown command: ");
        uart_puts(command);
        uart_puts("\r\n");
    }
}

void shell_run(void)
{
    char buffer[CMD_BUFFER_SIZE];

    uart_puts("\r\nType 'help' for available commands.\r\n");

    while (1) {
        int index = 0;

        uart_puts("os> ");

        while (1) {
            char c = uart_getc();

            /* Ignore LF. We use CR as Enter. */
            if (c == '\n') {
                continue;
            }

            if (c == '\r') {
                uart_puts("\r\n");
                buffer[index] = '\0';
                break;
            }

            /* Backspace / Delete */
            if (c == 8 || c == 127) {
                if (index > 0) {
                    index--;
                    uart_puts("\b \b");
                }
                continue;
            }

            /* Ignore other control characters */
            if (c < 32 || c > 126) {
                continue;
            }

            if (index < CMD_BUFFER_SIZE - 1) {
                buffer[index++] = c;
                uart_putc(c);
            }
        }

        execute_command(buffer);
    }
}
