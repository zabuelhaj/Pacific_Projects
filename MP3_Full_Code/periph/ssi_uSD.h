#ifndef _SSI_USD_H
#define _SSI_USD_H

#include <stdint.h>
#include <stdbool.h>

void initSSI0( void );
void setSSI0FastSpeed( bool spd );
void txByteSSI0( uint8_t data );
uint8_t rxByteSSI0( void );
void assertCS( bool on );

#endif // _SSI_USD_H
