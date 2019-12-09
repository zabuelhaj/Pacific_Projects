/*
 * motors.c
 *
 *  Created on: Feb 7, 2018
 *      Author: zabuelhaj & crash
 */

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "obstacle.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pwm.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "inc/hw_types.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

/* Default clock speed. */
#define CLOCK_RATE 16000000

/* Timer interrupt max value. */
#define TIM_MAX 320000

/* Radius of the robot. */
#define RADIUS 3

/* 2l. */
#define l2 12

/* Global variables. */
float time_elapsed;         // The time elapsed from Timer0A.
float pulsesR;              // The number of pulsesR counted on wheel rotation.
float pulsesL;              // For the left wheel now.
float temp_pulsesR;         // Holds the temporary number of pulsesR, gets reset every timeout.
float temp_pulsesL;         // For the left wheel now.
float wheel_speed;          // How fast the wheels move.
bool timeout_flag;          // Flag to indicate whether or not to read velocity.
bool leftDirection;         // Left wheel direction flag.
bool rightDirection;        // Right wheel direction flag.

/* Declare the Pose Object. */
typedef struct {
    float xt;
    float yt;
    float ot;
} Pose;

/* Global data structure that holds the global pose. */
Pose gPose = {0, 0, 0};
float previousPose = 0;

/* Return the global pose. */
void getPose (uint32_t *pose) {
    pose[0] = fabs(gPose.xt);
    pose[1] = fabs(gPose.yt);
    pose[2] = fabs(gPose.ot * 10);        // Multiply by 10 to keep accuracy for small values.
    if (gPose.xt < 0)
        pose[3] = 1;
    else
        pose[3] = 0;
    if (gPose.yt < 0)
        pose[4] = 1;
    else
        pose[4] = 0;
    if (gPose.ot < 0)
        pose[5] = 1;
    else
        pose[5] = 0;
}

/* Initialization procedures are below. */
/* These include initializing PWM, LED output, timers, and interrupts. */

void initPWM (void) {
    /* Set the system clock for PWM0 and PWM1. */
    SysCtlPeripheralEnable (SYSCTL_PERIPH_PWM0);
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_PWM0));
    SysCtlPeripheralEnable (SYSCTL_PERIPH_PWM1);
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_PWM1));

    /* Map PD0 to PWM1. */
    GPIOPinTypePWM (GPIO_PORTD_BASE, GPIO_PIN_0);
    GPIOPinConfigure (GPIO_PD0_M1PWM0);

    /* Map PB4 to PWM0. */
    GPIOPinTypePWM (GPIO_PORTB_BASE, GPIO_PIN_4);
    GPIOPinConfigure (GPIO_PB4_M0PWM2);

    /* Set the clock dividers for both modules. */
    PWMClockSet(PWM0_BASE, PWM_SYSCLK_DIV_2);
    PWMClockSet(PWM1_BASE, PWM_SYSCLK_DIV_2);

    /* Configure the PWM generators. */
    PWMGenConfigure (PWM1_BASE, PWM_GEN_0, (PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC));
    PWMGenConfigure (PWM0_BASE, PWM_GEN_1, (PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC));

    /* Set the period for both generators. This will be the same for both. */
    /* The value in the Load register will be arbitrary, HUB-ee wheels do not require specific period widths. */
    PWMGenPeriodSet (PWM1_BASE, PWM_GEN_0, 400);
    PWMGenPeriodSet (PWM0_BASE, PWM_GEN_1, 400);

    /* Set the default duty cycle for the wheels. */
    /* This value will change throughout the program to vary wheel speed. */
    PWMPulseWidthSet (PWM1_BASE, PWM_OUT_0, 399);       // ~--% duty cycle.
    PWMPulseWidthSet (PWM0_BASE, PWM_OUT_2, 399);       // ~--% duty cycle.

    /* Enable the PWM generators. */
    PWMGenEnable (PWM1_BASE, PWM_GEN_0);
    PWMGenEnable (PWM0_BASE, PWM_GEN_1);

    /* Enable the PWM output. */
    PWMOutputState (PWM1_BASE, PWM_OUT_0_BIT, true);
    PWMOutputState (PWM0_BASE, PWM_OUT_2_BIT, true);
}

void initLED (void) {
    /* Set the output direction for PF1. */
    GPIOPinTypeGPIOOutput (GPIO_PORTF_BASE, GPIO_PIN_1);
}

void initTimer (void) {
    /* Enable the Timer0 peripheral. */
    SysCtlPeripheralEnable (SYSCTL_PERIPH_TIMER0);

    /* Wait for the Timer0 module to be ready. */
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_TIMER0));

    /* Configure TimerA as a half-width one-shot timer. */
    TimerConfigure (TIMER0_BASE, (TIMER_CFG_PERIODIC_UP));
}

/* Start | stop the timer module. */
void startStopTimer (uint8_t stopGo) {
    /* If stopGo is high, enable timer. */
    if (stopGo == 1) {
        /* Set count time. */
        TimerLoadSet (TIMER0_BASE, TIMER_A, TIM_MAX);

        /* Enable the Timer0 module. */
        TimerEnable (TIMER0_BASE, TIMER_A);
    } else {
        /* Disable the Timer0 module. */
        TimerDisable (TIMER0_BASE, TIMER_A);
    }
}

/* The interrupt handler when reading phase pulsesR. */
void intHandlerPulsesR (void) {
    /* Get current GPIO value and XOR to toggle pin. */
    GPIOPinWrite (GPIO_PORTF_BASE, GPIO_PIN_1, (GPIOPinRead (GPIO_PORTF_BASE, GPIO_PIN_1) ^ GPIO_PIN_1));

    /* Increase the number of pulsesR. */
    temp_pulsesR++;

    /* Clear the interrupt. */
    GPIOIntClear (GPIO_PORTD_BASE, GPIO_INT_PIN_6);
}

/* The interrupt handler when reading phase pulsesL. */
void intHandlerPulsesL (void) {
    /* Increase the number of pulsesL. */
    temp_pulsesL++;

    /* Clear the interrupt. */
    GPIOIntClear (GPIO_PORTC_BASE, GPIO_INT_PIN_5);
}

/* The interrupt handler when reading timeout. */
void intHandlerTimer (void) {
    /* Stop the timer and get time elapsed. */
    time_elapsed = TIM_MAX;                                 // For accurate results, resist TimerValueGet (TIMER0_BASE, TIMER_A).
    time_elapsed = time_elapsed/CLOCK_RATE;                 // Units in seconds.

    /* Set the timeout flag and reset pulses. */
    timeout_flag = true;
    pulsesR = temp_pulsesR;
    pulsesL = temp_pulsesL;
    temp_pulsesR = 0;
    temp_pulsesL = 0;

    /* Clear the interrupt. */
    TimerIntClear (TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

void initInterruptTimer (void) {
    /* Register handler for timer A. */
    TimerIntRegister (TIMER0_BASE, TIMER_A, intHandlerTimer);

    /* Enable timer interrupt upon timeout. */
    TimerIntEnable (TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    /* Set timeout flag to false. */
    timeout_flag = false;

    /* IntMasterEnable () called in initInterruptpulsesR. */
}

void initInterruptPulsesL (void) {
    /* Make sure the GPIO pin is an input. */
    GPIOPinTypeGPIOInput (GPIO_PORTC_BASE, GPIO_PIN_5);

    /* Need to read GPIO PC5 rising and falling edge. */
    GPIOIntTypeSet (GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_RISING_EDGE | GPIO_FALLING_EDGE);

    /* Set the interrupt handler. */
    GPIOIntRegister (GPIO_PORTC_BASE, intHandlerPulsesL);

    /* Enable the GPIO pin PC5. */
    GPIOIntEnable (GPIO_PORTC_BASE, GPIO_INT_PIN_5);

    /* IntMasterEnable () called in initInterruptPulsesR (). */
}

void initInterruptPulsesR (void) {
    /* Make sure the GPIO pin is an input. */
    /* This will read in from the quadrature sensor. */
    GPIOPinTypeGPIOInput (GPIO_PORTD_BASE, GPIO_PIN_6);

    /* Need to read GPIO PD6 and PC5 rising and falling edge. */
    GPIOIntTypeSet (GPIO_PORTD_BASE, GPIO_PIN_6, GPIO_RISING_EDGE | GPIO_FALLING_EDGE);

    /* Set the interrupt handler. */
    GPIOIntRegister (GPIO_PORTD_BASE, intHandlerPulsesR);

    /* Enable the GPIO pins PD6. */
    GPIOIntEnable (GPIO_PORTD_BASE, GPIO_INT_PIN_6);

    IntMasterEnable ();
}

void initMotoDrivers (void) {
    /* Set the system clocks for GPIO ports B, C, D, E, and F. */
    /* After every SysCtl set, need to wait for the clock to set. */
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOB);
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_GPIOB));
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOC);
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_GPIOC));
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOD);
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_GPIOD));
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOE);
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_GPIOE));
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOF);
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_GPIOF));

    /* Set Pins D1-3 as outputs. After, enable the pins. */
    /* These are the first half of motor control outputs. */
    GPIOPinTypeGPIOOutput (GPIO_PORTD_BASE, (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3));

    /* Set Pins B1, B4, E4, and E5 as outputs. Enable them as well. */
    /* These are the second half of motor control outputs. */
    GPIOPinTypeGPIOOutput (GPIO_PORTB_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput (GPIO_PORTE_BASE, (GPIO_PIN_4 | GPIO_PIN_5));

    /* Initialize the PWM peripheral. */
    initPWM ();

    /* Initialize the LED that will blink during an interrupt handler. */
    initLED ();

    /* Initialize the timer for counting seconds. */
    initTimer ();

    /* Initialize the interrupts. */
    initInterruptTimer ();
    initInterruptPulsesL ();
    initInterruptPulsesR ();

    /* Set the wheel speed. */
    wheel_speed = 350;
}

/* Get the velocity from the right wheel. */
float getVelocityR (void) {

    /* Perform calculations. */
    return ((pulsesR*2*60)/(time_elapsed*64*9.5493));
}

/* Get the velocity from the left wheel. */
float getVelocityL (void){

    /* Perform calculations. */
    return ((pulsesL*2*60)/(time_elapsed*64*9.5493));
}

/* Get the distance traveled from the wheel. */
/* Check on every timeout where it is; stop when around desired distance. */
bool getDistance (float dest, uint8_t xyTurn, bool reset) {

    /* Delay to get more turning angle from robot. */
    if (xyTurn != 0x1) {
        for (uint16_t i = 0; i < 0xFFF; i++);
    }

    /* initialize the Pose structure. */
    static Pose pose = {0, 0, 0};                   // xt = 0, yt = 0, ot = 0.

    /* Reset the Pose and Pose struct after destination is "reached". */
    if (reset) {
        pose.xt = 0;
        pose.yt = 0;
        pose.ot = 0;
        return true;
    }

    /* Define other variables with scope within getDistance () function. */
    float phi1 = 0;                             // Right wheel velocity.
    float phi2 = 0;                             // Left wheel velocity.
    float wd = 0;                               // Omega * delta.
    bool retVal;                                // Return value.

    /* Get wheel velocity to find pose. */
    phi1 = getVelocityR ();
    phi2 = getVelocityL ();

    /* Adjust wheel velocity. */
    if (!leftDirection)
        phi2 = -phi2;
    if (!rightDirection)
        phi1 = -phi1;

    if (xyTurn == 0x1) {
        /* Apply FKM. */
        pose.xt = pose.xt + RADIUS*time_elapsed*phi1*cos (0);

        /* Calculate the global pose. */
        gPose.xt = gPose.xt + RADIUS*time_elapsed*phi1*cos(gPose.ot);
        gPose.yt = gPose.yt + RADIUS*time_elapsed*phi1*sin(gPose.ot);

        /* Check to see if the robot has reached it's destination. */
        if (fabs(pose.xt) >= dest) {
            /* Return true for correct x-coordinate, save desired pose. */
            retVal = true;
        } else {
            /* Return false for incorrect x or y coordinates. */
            retVal = false;
        }
    } else {

        /* Apply Modified FKM for rotating about point P. */
        wd = (RADIUS*phi1 - RADIUS*phi2)/l2;
        pose.ot = pose.ot + wd;
        gPose.ot = gPose.ot + wd;

        /* Check to see if the robot has turned the proper angle. */
        if (fabs(pose.ot) >= dest*55) {
            /* Return true, save desired pose value. */
            retVal = true;
            gPose.ot = gPose.ot/55 + previousPose;                 // Needs multiplier of 50 for increased accuracy. Remove this multiplier when saving pose.
            previousPose = gPose.ot;
        } else {
            /* It is not at the right angle yet. */
            retVal = false;
        }
    }

    return retVal;
}

/* Below are the procedures for motor control. */
/* These include but are not limited to: */
/* Motor forward, backward, brake, and standby. */

void motorForwardR(void) {
    /* Standby, IN2 low, IN1 high. */
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, ~GPIO_PIN_2);
}

void motorBackwardR(void){
    /* Standby, IN2 high, IN1 low. */
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, ~GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_PIN_2);
}

void motorBrakeR(void){
    /* Standby, IN1 high, IN2 high. */
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_PIN_2);
}

void motorStandbyR(void){
    /* Standby off, IN1 low, IN2 low. */
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, ~GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, ~GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, ~GPIO_PIN_2);
}

void motorForwardL(void) {
    /* Standby, IN2 low, IN1 high. */
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, GPIO_PIN_5);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, ~GPIO_PIN_4);
}

void motorBackwardL(void){
    /* Standby, IN2 high, IN1 low. */
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, GPIO_PIN_5);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, ~GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_PIN_4);
}

void motorBrakeL(void){
    /* Standby, IN1 high, IN2 high. */
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, GPIO_PIN_5);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_PIN_4);
}

void motorStandbyL(void){
    /* Standby off, IN1 low, IN2 low. */
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, ~GPIO_PIN_5);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, ~GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, ~GPIO_PIN_4);
}

void turnRobot (uint8_t direction, float distance) {
    uint32_t delay = 0;
    distance = distance/10;

    /* Stop the robot momentarily so it does not oversteer. */
    motorBrakeR ();
    motorBrakeL ();
    for (delay = 0; delay < 0xFFFF; delay++);

    /* Set the wheels to a different speed. */
    PWMPulseWidthSet (PWM1_BASE, PWM_OUT_0, 200);       // ~--% duty cycle.
    PWMPulseWidthSet (PWM0_BASE, PWM_OUT_2, 200);       // ~--% duty cycle.

    /* Turn the robot to the Y coordinate. */
    if (direction == 0x1) {

        /* Set the proper directions for both wheels. */
        leftDirection = true;
        rightDirection = false;

        /* Turn the robot upwards. */
        motorForwardR ();
        motorForwardL ();
    } else if (direction == 0x2) {

        /* Set the proper directions for both wheels. */
        leftDirection = false;
        rightDirection = true;

        /* Turn robot downwards. */
        motorBackwardR ();
        motorBackwardL ();
    } else
        return;

    startStopTimer(1);
    while (true) {
        if (timeout_flag == true) {
            timeout_flag = false;

            /* Check to see if robot turned proper amount... or etc. */
            if (getDistance (distance, 0x0, false)) {                                // Expected distance to travel. 0x0 = check turning angle.
                motorBrakeR ();
                motorBrakeL ();
                break;
            }
        }
    }
    startStopTimer(0);
    getDistance(distance, 0x0, true);
}

/* The user input functions. */
/* Accept an X and Y input and drive the motor towards the destination. */
void driveStraight (uint8_t direction, float distance) {
    uint32_t delay = 0;
    uint8_t obstacle;

    /* Stop the robot momentarily so it does not oversteer. */
    motorBrakeR ();
    motorBrakeL ();
    for (delay = 0; delay < 0xFFFF; delay++);

    /* Set the wheels to the "same" speed. */
    PWMPulseWidthSet (PWM1_BASE, PWM_OUT_0, wheel_speed);       // ~--% duty cycle.
    PWMPulseWidthSet (PWM0_BASE, PWM_OUT_2, wheel_speed-10);       // ~--% duty cycle.

    /* Assume the current position is (0, 0) and robot faces +X direction. */
    /* The initial position (0, 0) is just for Lab 3 demonstration. */
    if (direction == 0x1) {

        /* Set the proper directions for both wheels. */
        leftDirection = true;
        rightDirection = true;

        /* Move the robot forward. */
        motorForwardR ();
        motorBackwardL ();
    }
    else if (direction == 0x2) {

        /* Set the proper directions for both wheels. */
        leftDirection = false;
        rightDirection = false;

        /* Move the robot backwards. */
        motorBackwardR ();
        motorForwardL ();
    } else
        return;

    startStopTimer(1);
    while (true) {
        if (timeout_flag == true) {
            timeout_flag = false;
            obstacle = isObstacle();

            /* Check to see if robot has reached a checkpoint... or etc. */
            if (getDistance(distance, 0x1, false) || obstacle != 0x0) {                                  // 0x1 tells getDistance function to check X pose.
                motorBrakeR ();
                motorBrakeL ();

                /* Low level obstacle avoidance. */
                if (obstacle == 0x1) {
                    startStopTimer(0);
                    driveStraight((direction*-1), 15);
                    turnRobot(0x2, 1.6);
//                    startStopTimer(1);
                    break;
                } else if (obstacle == 0x2 || obstacle == 0x3) {
                    startStopTimer(0);
                    driveStraight((direction*-1), 15);
                    turnRobot(0x1, 1.6);
//                    startStopTimer(1);
                    break;
                } else
                    break;
            }
        }
    }
    startStopTimer(0);
    getDistance(distance, 0x0, true);
}

/* Set the motor speed, for high-level obstacle avoidance. */
void setSpeed (uint8_t mag) {
    /* Expect speed to be multiplied by 10. */
    float magnitude = abs((float) mag);
    magnitude = magnitude/10;

    /* Check to see if the magnitude lowers speed below 150. */
    /* Pulse widths set below 150/400 produces an immovable object. */
    if ((400*magnitude) < 150)
        wheel_speed = 150;
    else if ((400*magnitude) > 400)
        wheel_speed = 399;
    else
        wheel_speed = 400 * magnitude;
}
