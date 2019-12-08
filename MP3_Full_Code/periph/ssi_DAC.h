#ifndef _SSI_DAC_H
#define _SSI_DAC_H

#include <stdint.h>

void initSSI2( void );
void txDataSSI2( uint8_t dac, uint16_t data );

#endif // _SSI_DAC_H
