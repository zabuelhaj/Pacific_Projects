#include <stdint.h>
#include <stdbool.h>

#include "sysctl.h"
#include "ppb.h"
#include "gptm.h"

#define MAINOSCFREQ     120000000           // Overclocked value with osc.c source code.

// Configure Timer0A as a periodic timer.  You must also enable the interrupt.
void initTimer0A( void ) {
    /* Turn on timer 2. */
    SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGC_TIMER0;
    SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGC_TIMER0;

    /* Disable the timer for configuration. */
    GPTM_TIMER0[GPTM_CTL] &= ~GPTM_CTL_TAEN;

    /* Configure as a 32 bit timer. */
    GPTM_TIMER0[GPTM_CFG] &= ~GPTM_CFG_M;
    GPTM_TIMER0[GPTM_CFG] |= GPTM_CFG_32BITTIMER;

    /* Configure the timer as periodic. */
    GPTM_TIMER0[GPTM_TAMR] &= ~GPTM_TAMR_TAMR_M;
    GPTM_TIMER0[GPTM_TAMR] |= GPTM_TAMR_TAMR_PERIODIC;

    /* Allow the timer to use interrupts. */
    GPTM_TIMER0[GPTM_MIMR] |= GPTM_RIS_TATORIS;

    /* Allow the interrupt. */
    PPB[PPB_NVIC_EN0] |= PPB_NVIC_EN0_BIT19;
}

// Set the timer's reload value.  Rate is the frequency for the reload.
void setTimer0ARate( uint16_t rate ) {
    /* Set the reload value for a 120MHz clock. */
    GPTM_TIMER0[GPTM_TAILR] = ((MAINOSCFREQ*rate) - 1);       // x2 for 50% duty cycle.

    /* Clear the TimerA time-out flag. */
    GPTM_TIMER0[GPTM_ICR] = GPTM_ICR_TATOICR;
}

// Clear the time-out flag.
void clearTimer0A( void ) {
    /* Clear the TimerA time-out flag. */
    GPTM_TIMER0[GPTM_ICR] = GPTM_ICR_TATOICR;
}

// Enable/disable the timer.
void enableTimer0A( bool enabled ) {
    /* If enabled is true, enable the timer. */
    if(enabled)
        GPTM_TIMER0[GPTM_CTL] |= GPTM_CTL_TAEN | GPTM_CTL_TASTALL;
    else
        GPTM_TIMER0[GPTM_CTL] &= ~GPTM_CTL_TAEN;
}
