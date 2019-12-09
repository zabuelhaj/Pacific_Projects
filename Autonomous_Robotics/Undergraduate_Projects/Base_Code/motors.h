/*
 * motors.h
 *
 *  Created on: Feb 9, 2018
 *      Author: zabuelhaj
 */

#ifndef MOTORS_H_
#define MOTORS_H_

void getPose (uint32_t *pose);
void initPWM (void);
void initLED (void);
void initTimer (void);
void startStopTimer (uint8_t stopGo);
void intHandlerPulsesR (void);
void intHandlerPulsesL (void);
void intHandlerTimer (void);
void initInterruptTimer (void);
void initInterruptPulsesL (void);
void initInterruptPulsesR (void);
void initMotoDrivers (void);
float getVelocityR (void);
float getVelocityL (void);
bool getDistance (float dest, uint8_t xyTurn);
void motorForwardR (void);
void motorBackwardR (void);
void motorBrakeR (void);
void motorStandbyR (void);
void motorForwardL (void);
void motorBackwardL (void);
void motorBrakeL (void);
void motorStandbyL (void);
void driveStraight (uint8_t direction, float distance);
void turnRobot (uint8_t direction, float distance);
void setSpeed (uint8_t mag);

#endif /* MOTORS_H_ */
