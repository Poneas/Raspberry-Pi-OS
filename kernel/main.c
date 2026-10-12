#include "uart.h"
#include "shell.h"
#include "exception.h"
#include "gic.h"
#include "timer.h"
#include "memory.h"
#include "heap.h"
void kernel_main(void)
{
    uart_puts("\r\n");
    uart_puts("============================\r\n");
    uart_puts(" Raspberry-Pi-OS Kernel\r\n");
    uart_puts("============================\r\n");
    exception_init();
    uart_puts("ARM64 kernel initialized.\r\n");
    uart_puts("Exception vectors initialized.\r\n");
    gic_init();	
    uart_puts("GIC initialized.\r\n");
    timer_init();
    uart_puts("Timer initialized.\r\n");
    memory_init();
    uart_puts("memory manager initialized.\r\n");
    heap_init(
    memory_heap_start(),
    memory_heap_end()
);

uart_puts("Kernel heap initialized.\r\n");
    __asm__ volatile("msr daifclr, #2");
    uart_puts("Interrupts enabled.\r\n");
    shell_run();
    while (1) {
        __asm__ volatile("wfe");
    }
}
