/*
 * qssi.h
 *
 *  Created on: Oct 23, 2017
 *      Author: zabuelhaj
 */

#ifndef QSSI_H_
#define QSSI_H_

#include <stdint.h>
#include <stdbool.h>


#define QSSI0 ((volatile uint32_t *) 0x40008000)
#define QSSI2 ((volatile uint32_t *) 0x4000a000)

enum{
    SSICR0 = (0x000 >> 2),
    SSICR1 = (0x004 >> 2),
    SSE = (1<<1),
    SCR = (0xFF << 8),
    QSSI_DR = (0x008 >> 2),
    QSSI_SR = (0x00C >> 2),        //status
    QSSI_SR_BSY = (1<<4),   //busy bit in status register
    QSSI_SR_RNE = (1<<2),
    QSSI_SR_TNF = (1<<1),
    DSS = 0b1111,
    SPO = (1<<6),
    SPH = (1<<7),
    CPSDVR = (0x10 >> 2),
};


#endif /* QSSI_H_ */
