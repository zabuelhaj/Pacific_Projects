#ifndef _ID3_H
#define _ID3_H

#include <stdint.h>

#include "ff.h"

#define MAXTAGSIZE 128

struct id3tags {
  uint8_t title[MAXTAGSIZE];
  uint8_t artist[MAXTAGSIZE];
  uint8_t album[MAXTAGSIZE];
} tags;

uint32_t getID3Tags( FIL *fp, struct id3tags * fields );

#endif // _ID3_H
