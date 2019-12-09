/*
 * gptm.h: General-purpose Timer Module register definitions.  See Chapter 
 *  13 of the TM4C1294 datasheets for complete information.
 */

#ifndef _GPTM_H
#define _GPTM_H

#include <stdint.h>

#define GPTM_TIMER0             ((volatile uint32_t *)0x40030000)
#define GPTM_TIMER2             ((volatile uint32_t *)0x40032000)
#define GPTM_TIMER3             ((volatile uint32_t *)0x40033000)

enum {
  GPTM_CFG =    (0x000 >> 2),
#define GPTM_CFG_M              (0b111 << 0)    // GPTM Configuration
#define GPTM_CFG_32BITTIMER       (0 << 0)        // 32-bit timer
#define GPTM_CFG_32BITRTC         (1 << 0)        // 32-bit RTC
#define GPTM_CFG_16BITTIMERS      (4 << 0)        // 2 16-bit timers
  GPTM_TAMR =   (0x004 >> 2),
#define GPTM_TAMR_TAMR_M        (0b11 << 0)     // GPTM Timer A Mode
#define GPTM_TAMR_TAMR_ONESHOT    (1 << 0)        // One-Shot Timer mode
#define GPTM_TAMR_TAMR_PERIODIC   (2 << 0)        // Periodic Timer mode
#define GPTM_TAMR_TAMR_CAP        (3 << 0)        // Capture mode
  GPTM_CTL =    (0x00c >> 2),
#define GPTM_CTL_TASTALL        (1 << 1)        // GPTM Timer A Stall Enable
#define GPTM_CTL_TAEN           (1 << 0)        // GPTM Timer A Enable
  GPTM_RIS =    (0x01c >> 2),
#define GPTM_RIS_TATORIS        (1 << 0)        // GPTM Timer A Time-Out
                                                //   Interrupt Mask
  GPTM_ICR =    (0x024 >> 2),
#define GPTM_ICR_TATOICR        GPTM_RIS_TATORIS
#define GPTM_ICR_TATOCINT       GPTM_RIS_TATORIS
  GPTM_TAILR =  (0x028 >> 2),
  GPTM_TAPR =   (0x038 >> 2),
#define GPTM_TAPR_TAPSR_M       (0xff << 0)     // GPTM Timer A Prescale
#define GPTM_TAPR_TAPSR_set     (0x9<<0)
  GPTM_MIMR =   (0x018 >> 2),                   // GPTM Interrupt Mask.
#define GPTM_MIMR_TATOIM        GPTM_RIS_TATORIS
#define GPTM_IMR_TATOIM         GPTM_RIS_TATORIS
   GPTM_IMR = (0x18 >> 2),
};

#endif // _GPTM_H
