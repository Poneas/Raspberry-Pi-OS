#ifndef GIC_H
#define GIC_H

void gic_init(void);
unsigned int gic_acknowledge(void);
void gic_end_interrupt(unsigned int irq);

#endif
