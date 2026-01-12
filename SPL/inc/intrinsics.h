#ifndef __INTRINSICS_H
#define __INTRINSICS_H

/* SDCC doesn't need intrinsics.h, but we provide these for compatibility */

/* Interrupt enable/disable */
#define enableInterrupts()    __asm__("rim")
#define disableInterrupts()   __asm__("sim")

/* No operation */
#define nop()                 __asm__("nop")

/* Wait for interrupt */
#define wfi()                 __asm__("wfi")

/* Wait for event */
#define wfe()                 __asm__("wfe")

/* Halt */
#define halt()                __asm__("halt")

/* Trap - software interrupt */
#define trap()                __asm__("trap")

#endif /* __INTRINSICS_H */