#ifndef _TIMER2A_H
#define _TIMER2A_H

#include <stdint.h>

void initTimer2A( void );
void setTimer2ARate( uint16_t rate );
void clearTimer2A( void );
void enableTimer2A( bool enabled );

#endif // _TIMER2A_H
