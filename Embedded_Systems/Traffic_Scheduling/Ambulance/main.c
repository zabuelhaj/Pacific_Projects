

/**
 * main.c
 */

#include <stdint.h>
#include <stdbool.h>
#include "sysctl.h"
#include "systick.h"
#include "ppb.h"
#include "osc.h"
#include "gpio.h"
#include "timer0A.h"

#define     DELAYCONST     1200000

/* Global event triggers. */
bool amb_enter_avenue = false;
bool amb_enter_street = false;
bool amb_exit = false;
bool change_lights = true;
bool A = false;

/* Event prototypes. */
void turn_red (bool street_ave);
void turn_green (bool street_ave);

/* Prototype interrupt handlers. */
void amb_enter_road (void);
void button_pressed (void);

int main(void) {
    /* Initialize counter variable. */
    uint32_t i = 0;

    /* Unit test to make sure systick works properly. */
    /* Overclock the Microcontroller. */
    initOsc ();

    /* Set the timer. */
    initTimer0A ();

    /* Set system clock for Port N. */
    SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTN | SYSCTL_RCGCGPIO_PORTF | SYSCTL_RCGCGPIO_PORTJ;
    SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTN | SYSCTL_RCGCGPIO_PORTF | SYSCTL_RCGCGPIO_PORTJ;           // Once more to ensure it took.

    /* Configure the GPIO ports to use the four LEDs. */
    GPIO_PORTN[GPIO_DIR] |= GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_PORTN[GPIO_DEN] |= GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_PORTN[GPIO_PIN_0] &= ~GPIO_PIN_0;                      // Make sure the LED1 is off.
    GPIO_PORTN[GPIO_PIN_1] &= ~GPIO_PIN_1;                      // Make sure the LED0 is off.

    GPIO_PORTF[GPIO_DIR] |= GPIO_PIN_0 | GPIO_PIN_4;
    GPIO_PORTF[GPIO_DEN] |= GPIO_PIN_0 | GPIO_PIN_4;
    GPIO_PORTF[GPIO_PIN_0] &= ~GPIO_PIN_0;                      // Make sure the LED3 is off.
    GPIO_PORTF[GPIO_PIN_4] &= ~GPIO_PIN_4;                      // Make sure the LED2 is off.

    /* Configure the pull-up resistor for the Port J switches. */
    /* Enable interrupts on Port J. */
    GPIO_PORTJ[GPIO_DIR] &= ~(GPIO_PIN_1 | GPIO_PIN_0);
    GPIO_PORTJ[GPIO_PUR] |= GPIO_PIN_1 | GPIO_PIN_0;
    GPIO_PORTJ[GPIO_IMR] |= (GPIO_PORTJ[GPIO_IMR] & ~GPIO_IMR_M) | GPIO_PUSH_BUTTONS;
    PPB[PPB_NVIC_EN1] |= PPB_NVIC_EN1_BIT_51;
    GPIO_PORTJ[GPIO_DEN] |= GPIO_PIN_1 | GPIO_PIN_0;

    /* Configure the SysTick. */
    configSysTick ();
    enableSysTick (true);

    /* State names for the finite state machine. */
    enum {
        AVENUE_GREEN_STREET_RED, AVENUE_RED_STREET_GREEN
    };

    /* Initialize the state machine. */
    uint8_t state = AVENUE_GREEN_STREET_RED;

    /* When the delay is complete, flash the LED. */
    while (true) {
        /* Implement a simple FSM. */
        switch (state) {
        case AVENUE_GREEN_STREET_RED:
            if (!A && change_lights) {
                turn_red(false);
                turn_green(true);
                for (i = 0; i < 1000; i++) {                   // 1000*0.01 sec= 10 seconds.
                    sysTickBusy (DELAYCONST);               // Wait for ( 1200000 /120000000= 0.01 sec) = 10 ms.
                    if (A) {
                        if (amb_enter_avenue) {
                            GPIO_PORTN[GPIO_PIN_1] &= ~GPIO_PIN_1;
                            GPIO_PORTN[GPIO_PIN_0] |= GPIO_PIN_0;
                            state = AVENUE_GREEN_STREET_RED;
                        } else {
                            GPIO_PORTN[GPIO_PIN_0] &= ~GPIO_PIN_0;
                            GPIO_PORTN[GPIO_PIN_1] |= GPIO_PIN_1;
                            state = AVENUE_RED_STREET_GREEN;
                        }
                        break;
                    }
                }
                state = AVENUE_RED_STREET_GREEN;
            } else {
                if (amb_enter_avenue) {
                    setTimer0ARate(5);
                    GPIO_PORTN[GPIO_PIN_1] &= ~GPIO_PIN_1;
                    GPIO_PORTN[GPIO_PIN_0] |= GPIO_PIN_0;
                    state = AVENUE_GREEN_STREET_RED;
                } else {
                    setTimer0ARate(3);
                    GPIO_PORTN[GPIO_PIN_0] &= ~GPIO_PIN_0;
                    GPIO_PORTN[GPIO_PIN_1] |= GPIO_PIN_1;
                    state = AVENUE_RED_STREET_GREEN;
                }
                turn_green(true);
                turn_green(false);
                enableTimer0A(true);
                while (!amb_exit && A) {
                    GPIO_PORTF[GPIO_PIN_1] ^= GPIO_PIN_1;
                    GPIO_PORTF[GPIO_PIN_0] ^= GPIO_PIN_0;
                    for (i = 0; i < 0xFFFFF; i++);
                }
                turn_red(true);
                turn_red(false);
                GPIO_PORTN[GPIO_PIN_1] &= ~GPIO_PIN_1;
                GPIO_PORTN[GPIO_PIN_0] &= ~GPIO_PIN_0;
                enableTimer0A(false);
            }
        case AVENUE_RED_STREET_GREEN:
            if (!A && change_lights) {
                turn_red(true);
                turn_green(false);
                for (i = 0; i < 1000; i++) {                   // 1000*0.01 sec= 10 seconds.
                    sysTickBusy (DELAYCONST);               // Wait for ( 1200000 /120000000= 0.01 sec) = 10 ms.
                    if (A) {
                        if (amb_enter_avenue) {
                            GPIO_PORTN[GPIO_PIN_1] &= ~GPIO_PIN_1;
                            GPIO_PORTN[GPIO_PIN_0] |= GPIO_PIN_0;
                            state = AVENUE_GREEN_STREET_RED;
                        } else {
                            GPIO_PORTN[GPIO_PIN_0] &= ~GPIO_PIN_0;
                            GPIO_PORTN[GPIO_PIN_1] |= GPIO_PIN_1;
                            state = AVENUE_RED_STREET_GREEN;
                        }
                        break;
                    }
                }
                state = AVENUE_GREEN_STREET_RED;
            } else {
                if (amb_enter_avenue) {
                    setTimer0ARate(5);
                    GPIO_PORTN[GPIO_PIN_1] &= ~GPIO_PIN_1;
                    GPIO_PORTN[GPIO_PIN_0] |= GPIO_PIN_0;
                    state = AVENUE_GREEN_STREET_RED;
                } else {
                    setTimer0ARate(3);
                    GPIO_PORTN[GPIO_PIN_0] &= ~GPIO_PIN_0;
                    GPIO_PORTN[GPIO_PIN_1] |= GPIO_PIN_1;
                    state = AVENUE_RED_STREET_GREEN;
                }
                turn_green(true);
                turn_green(false);
                enableTimer0A(true);
                while (!amb_exit && A) {
                    GPIO_PORTF[GPIO_PIN_4] ^= GPIO_PIN_4;
                    GPIO_PORTF[GPIO_PIN_0] ^= GPIO_PIN_0;
                    for (i = 0; i < 0xFFFFF; i++);
                }
                turn_red(true);
                turn_red(false);
                GPIO_PORTN[GPIO_PIN_1] &= ~GPIO_PIN_1;
                GPIO_PORTN[GPIO_PIN_0] &= ~GPIO_PIN_0;
                enableTimer0A(false);
            }
        }
    }

}

/* The turn_red command is controlled by the event change_lights. */
/* This will change the current LED to the opposite color that is currently is. */
void turn_red (bool street_ave) {
    if (street_ave)                                 // True indicates street is red.
        GPIO_PORTF[GPIO_PIN_4] &= ~GPIO_PIN_4;
    else                                            // False indicates avenue is red.
        GPIO_PORTF[GPIO_PIN_0] &= ~GPIO_PIN_0;
}

/* The turn_green command is similar to the turn_red command. */
/* The difference here is that the command will perform the opposite of turn_red. */
void turn_green (bool street_ave) {
    if (street_ave)                                 // True indicates street is green.
        GPIO_PORTF[GPIO_PIN_4] |= GPIO_PIN_4;
    else                                            // False indicates avenue is green.
        GPIO_PORTF[GPIO_PIN_0] |= GPIO_PIN_0;
}

void amb_enter_road (void) {
    /* Reset the flags and clear interrupt. */
    A = false;
    amb_exit = true;
    change_lights = true;
    amb_enter_avenue = false;
    amb_enter_street = false;

    /* Clear the timeout flag on the Timer0A peripheral. */
    clearTimer0A ();
}

void button_pressed (void) {
    /* Check to see if a button is pushed (an ambulance drives through). */
    if (GPIO_PORTJ[GPIO_PIN_0] != GPIO_PIN_0) {
        A = true;
        amb_enter_avenue = true;
        amb_enter_street = false;
        change_lights = false;
        amb_exit = false;
    } else if (GPIO_PORTJ[GPIO_PIN_1] != GPIO_PIN_1) {
        A = true;
        amb_enter_avenue = false;
        amb_enter_street = true;
        change_lights = false;
        amb_exit = false;
    }
    GPIO_PORTJ[GPIO_ICR] |= GPIO_ICR_M;
}
