/*
 * UART3.h
 *
 *  Created on: Mar 21, 2018
 *      Author: zabuelhaj
 */

#ifndef UART3_H_
#define UART3_H_

void initUART3 (void);
void getStartByteUART3 (void);
uint8_t readByteUART3 (void);
void sendByteUART3 (uint8_t byte);
void sendPacketUART3 (uint8_t packet[], uint8_t len);

#endif /* UART3_H_ */
