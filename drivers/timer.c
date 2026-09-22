#include "timer.h"

static volatile unsigned long ticks = 0;

static unsigned long timer_frequency = 0;
static unsigned long ticks_per_interrupt = 0;

void timer_init(void)
{
    /*
     * CNTFRQ_EL0 contains the system counter frequency.
     */
    __asm__ volatile(
        "mrs %0, CNTFRQ_EL0"
        : "=r"(timer_frequency)
    );

    /*
     * Generate 100 timer interrupts per second.
     */
    ticks_per_interrupt = timer_frequency / 100;

    __asm__ volatile(
        "msr CNTP_TVAL_EL0, %0"
        :
        : "r"(ticks_per_interrupt)
    );

    /*
     * CNTP_CTL_EL0:
     * bit 0 = ENABLE
     * bit 1 = IMASK
     *
     * 1 = enabled, interrupt not masked.
     */
    unsigned long control = 1;

    __asm__ volatile(
        "msr CNTP_CTL_EL0, %0"
        :
        : "r"(control)
    );
}

void timer_handle_irq(void)
{
    ticks++;

    /*
     * Schedule next timer interrupt.
     */
    __asm__ volatile(
        "msr CNTP_TVAL_EL0, %0"
        :
        : "r"(ticks_per_interrupt)
    );
}

unsigned long timer_get_ticks(void)
{
    return ticks;
}

unsigned long timer_get_seconds(void)
{
    return ticks / 100;
}
