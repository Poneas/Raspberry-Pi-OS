#ifndef TIMER_H
#define TIMER_H

void timer_init(void);
void timer_handle_irq(void);

unsigned long timer_get_ticks(void);
unsigned long timer_get_seconds(void);

#endif

