/*
 * commandControl.c
 *
 *  Created on: Mar 22, 2018
 *      Author: zabuelhaj
 */

#include <stdint.h>
#include <stdbool.h>
#include "motors.h"
#include "bumper.h"
#include "IR.h"
#include "obstacle.h"

/*
 * This library file takes user input from the Raspberry Pi
 * and has robot respond accordingly.
 */

uint8_t doCommand (uint8_t command, uint8_t parameter, uint32_t *Dist, uint32_t *Pose) {
    /* Case statement to identify user input. */
    switch (command) {
    case 0x00:          // Brake!
        driveStraight(0x0, (float)parameter);
        return 0xFF;
    case 0x01:          // Drive forward!
        driveStraight(0x1, (float)parameter);
        return 0xFF;
    case 0x02:          // Drive backwards!
        driveStraight(0x2, (float)parameter);
        return 0xFF;
    case 0x03:          // Turn right!
        turnRobot(0x1, (float)parameter);
        return 0xFF;
    case 0x04:          // Turn left!
        turnRobot(0x2, (float)parameter);
        return 0xFF;
    case 0x05:          // Read both bumpers!
        return (uint8_t) getBumperStatus();
    case 0x06:          // Read right IR sensor.
        getIRDistance(Dist);
        return (uint8_t) Dist[1];
    case 0x07:          // Read center IR sensor.
        getIRDistance(Dist);
        return (uint8_t) Dist[0];
    case 0x08:          // Read left IR sensor.
        getIRDistance(Dist);
        return (uint8_t) Dist[2];
    case 0x09:          // Return distance to closest obstacle.
        getIRDistance(Dist);
        if (Dist[0] < Dist[1] && Dist[0] < Dist[2])
            return (uint8_t) Dist[0];
        else if (Dist[1] < Dist[0] && Dist[1] < Dist[2])
            return (uint8_t) Dist[1];
        else
            return (uint8_t) Dist[2];
    case 0x0A:          // Return location in global plane.
        getPose(Pose);
        return 0xFF;
    case 0x0B:          // Set the speed.
        setSpeed(parameter);
        return 0xFF;
    case 0x0C:          // Turn on/off obstacle avoidance (low level).
        noAvoidance();
        return 0xFF;
    }

    /* If case is not met, return 0x00. */
    return 0x00;

}
