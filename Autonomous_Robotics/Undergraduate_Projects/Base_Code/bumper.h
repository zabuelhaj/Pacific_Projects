/*
 * header.h
 *
 *  Created on: Feb 25, 2018
 *      Author: calebrash
 */

#ifndef BUMPER_H_
#define BUMPER_H_
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

uint32_t bumperTrigger;

void initBumpers();
void bumperHandler();
int getBumperStatus();



#endif /* BUMPER_H_ */
