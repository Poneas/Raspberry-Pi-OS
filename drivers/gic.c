#include "gic.h"

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

#define GICD_BASE 0x08000000UL
#define GICC_BASE 0x08010000UL

#define GICD_CTLR           (*(volatile uint32_t *)(GICD_BASE + 0x000))
#define GICD_ISENABLER0     (*(volatile uint32_t *)(GICD_BASE + 0x100))

#define GICC_CTLR           (*(volatile uint32_t *)(GICC_BASE + 0x000))
#define GICC_PMR            (*(volatile uint32_t *)(GICC_BASE + 0x004))
#define GICC_IAR            (*(volatile uint32_t *)(GICC_BASE + 0x00C))
#define GICC_EOIR           (*(volatile uint32_t *)(GICC_BASE + 0x010))

#define GENERIC_TIMER_IRQ 30

void gic_init(void)
{
    /*
     * Enable ARM generic non-secure physical timer PPI.
     * IRQ ID = 30.
     */
    GICD_ISENABLER0 = (1U << GENERIC_TIMER_IRQ);

    /*
     * Accept all interrupt priorities.
     */
    GICC_PMR = 0xFF;

    /*
     * Enable CPU interface and distributor.
     */
    GICC_CTLR = 1;
    GICD_CTLR = 1;
}

unsigned int gic_acknowledge(void)
{
    return GICC_IAR;
}

void gic_end_interrupt(unsigned int irq)
{
    GICC_EOIR = irq;
}
