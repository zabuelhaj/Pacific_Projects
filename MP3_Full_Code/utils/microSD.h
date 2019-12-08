#ifndef _MICROSD_H
#define _MICROSD_H

#include <stdint.h>
#include <stdbool.h>

bool initMicroSD( void );
bool readBlock( uint32_t blockaddr, uint8_t *buffer );
#endif  // _MICROSD_H
