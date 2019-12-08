#ifndef _FILEUTILS_H
#define _FILEUTILS_H

#include <stdint.h>

#include "ff.h"

uint8_t countMP3( void );
bool findMP3( uint8_t index, FIL *fp );

#endif // _FILEUTILS_H
