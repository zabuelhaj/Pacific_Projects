#ifndef __KEYPAD_H__
#define __KEYPAD_H__

#include <stdint.h>
#include <stdbool.h>

void initKeypad( const uint8_t lookup[4][4] );
bool getKey( uint8_t* key );

#endif //  __KEYPAD_H__
