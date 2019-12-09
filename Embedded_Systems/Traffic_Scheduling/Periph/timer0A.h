#ifndef _TIMER2A_H
#define _TIMER2A_H

#include <stdint.h>

void initTimer0A( void );
void setTimer0ARate( uint16_t rate );
void clearTimer0A( void );
void enableTimer0A( bool enabled );

#endif // _TIMER2A_H
