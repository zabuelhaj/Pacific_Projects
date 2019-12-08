#ifndef _LCD_H
#define _LCD_H

#include <stdint.h>

void initLCD( void );
void clearLCD ( void );
void positionLCD ( uint8_t y, uint8_t x );
void stringLCD ( const char *str ); //was uint 8

#endif // _LCD_H
