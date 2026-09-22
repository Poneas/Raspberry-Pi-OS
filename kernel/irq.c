#include "gic.h"
#include "timer.h"

#define GENERIC_TIMER_IRQ 30

void irq_handler(void)
{
    unsigned int iar = gic_acknowledge();
    unsigned int irq = iar & 0x3FF;

    if (irq == GENERIC_TIMER_IRQ) {
        timer_handle_irq();
    }

    gic_end_interrupt(iar);
}
