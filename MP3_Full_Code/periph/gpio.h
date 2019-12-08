/*
 * gpio.h
 *
 *  Created on: Sep 11, 2017
 *      Author: zabuelhaj
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

#define GPIO_PORTA      (((volatile uint32_t *) 0x40058000))
#define GPIO_PORTD      (((volatile uint32_t *) 0x4005b000))
#define GPIO_PORTE      (((volatile uint32_t *) 0x4005c000))
#define GPIO_PORTJ      (((volatile uint32_t *) 0x40060000))
#define GPIO_PORTF      (((volatile uint32_t *) 0x4005D000))
#define GPIO_PORTK      (((volatile uint32_t *) 0x40061000))

enum {
    GPIO_DATA = 0x000,
    GPIO_DIR = (0x400 >> 2),
    GPIO_PUR = (0x510 >> 2),
    GPIO_DEN = (0x51c >> 2),
    GPIO_LOCK = (0x520 >> 2),
    GPIO_CR = (0x524 >> 2),
    GPIO_AFSEL = (0x420 >> 2),
    GPIO_PCTL = (0x52c >> 2),
};

enum {
    GPIO_PIN_0 = (1<<0),
    GPIO_PIN_1 = (1<<1),
    GPIO_PIN_2 = (1<<2),
    GPIO_PIN_3 = (1<<3),
    GPIO_PIN_4 = (1<<4),
    GPIO_PIN_5 = (1<<5),
    GPIO_ALLPINS = 0xff,
    GPIO_ALL8 = 0xFF,
};


#endif /* GPIO_H_ */
