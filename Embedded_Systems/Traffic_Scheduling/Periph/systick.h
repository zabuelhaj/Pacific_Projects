/*
 * systick.h
 *
 *  Created on: Feb 27, 2018
 *      Author: zabuelhaj
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

void configSysTick (void);
void setReload (void);
void enableSysTick (bool enabled);
void sysTickBusy (uint32_t delay);

#endif /* SYSTICK_H_ */
