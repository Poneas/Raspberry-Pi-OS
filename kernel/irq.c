#include "gic.h"
#include "timer.h"
#include "scheduler.h"
#define GENERIC_TIMER_IRQ 30

unsigned long irq_handler(unsigned long frame_sp)
{
    unsigned int iar = gic_acknowledge();
    unsigned int irq = iar & 0x3FF;

    unsigned long next_frame = frame_sp;

    if (irq == GENERIC_TIMER_IRQ) {

        timer_handle_irq();

        next_frame =
            scheduler_on_timer(frame_sp);
    }

    gic_end_interrupt(iar);

    return next_frame;
}
