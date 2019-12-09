/*
 * systick.c
 *
 *  Created on: Feb 26, 2018
 *      Author: zabuelhaj
 */

#include <stdint.h>
#include <stdbool.h>
#include "ppb.h"

void configSysTick (void) {
    /* Explicitly disable the SysTick timer. */
    PPB[PPB_STCTRL] &= ~PPB_STCTRL_ENABLE;

    /* Configure the clock source to the System Clock. */
    PPB[PPB_STCTRL] |= PPB_STCTRL_CLK_SRC;

    /* Set the reload value to equate 10 seconds. */
    PPB[PPB_STRELOAD] |= (PPB[PPB_STRELOAD] & ~PPB_RELOAD_M) | PPB_RELOAD_M;

    /* Clear the STCURRENT register. */
    PPB[PPB_STCURRENT] &= ~PPB_RELOAD_M;
}

void setReload (void) {
    /* Set the reload value to equate 10 seconds. */
    PPB[PPB_STRELOAD] |= (PPB[PPB_STRELOAD] & ~PPB_RELOAD_M) | PPB_RELOAD_M;
    /* Clear the STCURRENT register. */
    PPB[PPB_STCURRENT] &= ~PPB_RELOAD_M;
}

void enableSysTick (bool enabled) {
    if (enabled)
        PPB[PPB_STCTRL] |= PPB_STCTRL_ENABLE;
    else
        PPB[PPB_STCTRL] &= ~PPB_STCTRL_ENABLE;
}

void sysTickBusy (uint32_t delay) {
    volatile uint32_t elapsedTime;
    uint32_t startTime = PPB[PPB_STCURRENT];
    do{
    elapsedTime = (startTime-PPB[PPB_STCURRENT]) & PPB_RELOAD_M;
    }
    while(elapsedTime <= delay);
}
