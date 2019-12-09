/*
 * obstacle.c
 *
 *  Created on: Mar 28, 2018
 *      Author: zabuelhaj
 */

#include <stdint.h>
#include "IR.h"
#include "bumper.h"

/* Global variable to to toggle obstacle avoidance. */
uint8_t avoid = 0;      // Active low.

uint8_t isObstacle (void) {
    /* Declare local variables. */
    uint8_t bumper = 0;
    uint32_t Dist[3];

    bumper = getBumperStatus();
    getIRDistance(Dist);
    if (bumper)
        return bumper;          // Bumper will indicate which direction to turn.
    else if (Dist[1] < 15 && avoid == 0) {
        if (Dist[2] < Dist[0])
            return 0x1;         // Turn right.
        else
            return 0x2;         // Turn left.
    } else
        return 0x0;

}

void noAvoidance (void) {
    /* Toggle obstacle avoidance on/off. */
    avoid = avoid ^ 1;
}
