/*
 * UART3.c
 *
 *  Created on: Mar 21, 2018
 *      Author: zabuelhaj
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"

/* UART3 uses pins PC6 and PC7. */

/* Procedure to initialize the UART Peripheral and its corresponding GPIO Port. */
void initUART3 (void) {
    /* Set the system clock for the UART module and the GPIO port to be used. */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_UART3));
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_GPIOC));

    /* Configure the GPIO pins for their alternate functions. */
    GPIOPinConfigure(GPIO_PC6_U3RX);
    GPIOPinConfigure(GPIO_PC7_U3TX);
    GPIOPinTypeUART(GPIO_PORTC_BASE, (GPIO_PIN_6 | GPIO_PIN_7));

    /* Continue configuring the UART. */
    UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), 9600, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
}

/* Procedure to find start byte. */
void getStartByteUART3 (void) {
    /* Wait for data to be present from the UART receive FIFO. */
    while(!UARTCharsAvail(UART3_BASE));

    /* Read a character from the receive FIFO. */
    while (true) {
        if (UARTCharGet(UART3_BASE) == 0xAA)                   // If a start byte is found, then read the next 3 bytes.
            break;
    }
}

/* Procedure to return a single "byte" of data. */
uint8_t readByteUART3 (void) {
    /* Return a single byte of data. */
    return UARTCharGet(UART3_BASE);
}

/* Procedure to send a single byte to the RPi. */
void sendByteUART3 (uint8_t byte) {
    /* Send byte through UART. */
    UARTCharPut(UART3_BASE, byte);
}

/* Procedure to send an entire data packet. */
void sendPacketUART3 (uint8_t packet[], uint8_t len) {
    /* Counter variable i. */
    uint8_t i = 0;

    /* Send packet starting with 0xAA and ending with 0x55. */
    for (i = 0; i < len; i++)
        UARTCharPut(UART3_BASE, packet[i]);
}
