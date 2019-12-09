/*
 * main.c
 *
 *  Created on: Mar 21, 2018
 *      Author: zabuelhaj
 */

#include <stdint.h>
#include <stdbool.h>
#include "UART3.h"
#include "commandControl.h"
#include "motors.h"
#include "bumper.h"
#include "IR.h"

/* Receive some data from the UART3 module. */

int main (void) {
    /* Local variable declaration. */
    volatile uint8_t data[2];
    uint8_t packet[] = {0xAA, 0, 0x55};
    uint8_t posePacket[] = {0xAA, 0, 0, 0, 0, 0, 0, 0x55};
    uint32_t Dist[] = {0, 0, 0};
    uint32_t Pose[] = {0, 0, 0, 0, 0, 0};
    uint8_t tempData = 0;
    uint8_t i = 0;

    /* Initialize the UART module. */
    initUART3();

    /* Initialize the sensors. */
    initADC();
    initBumpers();

    /* Initialize the motor drivers. */
    initMotoDrivers ();

    while (true) {
        /* Wait to see something from the Raspberry Pi. */
        getStartByteUART3();

        /* Read two bytes from the UART FIFO. */
        while (true) {
            tempData = readByteUART3();
            if (tempData == 0x55)
                break;
            else {
                data[i] = tempData;
                i++;
            }
        }
        /* Reset i variable. */
        i = 0;

        /* Read the incoming packet and get a value to send to Raspberry Pi. */
        packet[1] = doCommand(data[0], data[1], Dist, Pose);

        /* Send the packet (if there is data, anyway). */
        if (data[0] == 0x0A) {
            posePacket[1] = (uint8_t) Pose[0];
            posePacket[2] = (uint8_t) Pose[1];
            posePacket[3] = (uint8_t) Pose[2];
            posePacket[4] = (uint8_t) Pose[3];
            posePacket[5] = (uint8_t) Pose[4];
            posePacket[6] = (uint8_t) Pose[5];
            sendPacketUART3(posePacket, 0x8);
        } else
            sendPacketUART3(packet, 0x3);
    }
}
