//
// NHD-C12864WC-FSW-FBW-3V3 LCD driver
//

// 8080 (Intel) mode parallel interface.

#include <stdint.h>
#include <stdbool.h>

#include "sysctl.h"
#include "gpio.h"
#include "ppb.h"
#include "font.h"
#include "lcd.h"

// Dimensions of the LCD module
#define LCD_HEIGHT               64
#define LCD_WIDTH               128
#define LINES_PER_PAGE            8  // each page is 8 lines

// ST7565R commands and bitmasks
#define ST7565R_DISPLAY_OFF                     (0xae | 0b0)
#define ST7565R_DISPLAY_ON                      (0xaf | 0b1)
#define ST7565R_DISPLAY_START_LINE_SET          0x40
#define   ST7565R_DISPLAY_START_LINE_SET_M        0b111111
#define ST7565R_PAGE_ADDRESS_SET                0xb0
#define   ST7565R_PAGE_ADDRESS_SET_M              0b1111
#define ST7565R_COLUMN_ADDRESS_SET_HIGHBITS     0x10
#define ST7565R_COLUMN_ADDRESS_SET_LOWBITS      0x00
#define   ST7565R_COLUMN_ADDRESS_SET_M            0b1111
#define ST7565R_ADC_SELECT_NORMAL               (0xa0 | 0b0)
#define ST7565R_ADC_SELECT_REVERSE              (0xa0 | 0b1)
#define ST7565R_DISPLAY_ALL_POINTS_OFF          0xa4
#define ST7565R_DISPLAY_ALL_POINTS_ON           0xa5
#define ST7565R_LCD_BIAS_SET                    0xa2
#define ST7565R_READ_MODIFY_WRITE               0xe0
#define ST7565R_READ_MODIFY_WRITE_END           0xee
#define ST7565R_RESET                           0xe2
#define ST7565R_COMMON_OUT_MODE_SELECT_NORMAL   0xc0
#define ST7565R_COMMON_OUT_MODE_SELECT_REVERSE  0xc8
#define ST7565R_POWER_CONTROL_SET               0x28
#define   ST7565R_POWER_CONTROL_SET_M             0b111
#define ST7565R_INTERNAL_RESISTOR_RATIO_SET     0x20
#define ST7565R_ELECTRONIC_VOLUME_MODE_SET      0x81
#define ST7565R_SLEEP_MODE_SET_SLEEP            (0xac | 0b0)
#define ST7565R_SLEEP_MODE_SET_NORMAL           (0xac | 0b1)
#define ST7565R_BOOSTER_RATION_SELECT_SET       0xf8

// Pin assignments for device.
#define A0_PIN          GPIO_PIN_0
#define RD_PIN          GPIO_PIN_1
#define WR_PIN          GPIO_PIN_2
#define CS_PIN          GPIO_PIN_3
#define RST_PIN         GPIO_PIN_4

// Macros for controlling signal pins
#define assertReset(val) {GPIO_PORTF[RST_PIN] = (val) ? 0x00 : 0xff;}
#define assertCS(val) {GPIO_PORTF[CS_PIN] = (val) ? 0x00 : 0xff;}
#define assertWR(val) {GPIO_PORTF[WR_PIN] = (val) ? 0x00 : 0xff;}
#define assertRD(val) {GPIO_PORTF[RD_PIN] = (val) ? 0x00 : 0xff;}
#define A0Low()       {GPIO_PORTF[A0_PIN] = 0;}
#define A0High()      {GPIO_PORTF[A0_PIN] =  A0_PIN;}
#define writeDB(val)  {GPIO_PORTK[0xff] = val;}

// Write one byte to LCD.  
static void writeLCD( uint8_t c ) {
//assertReset(), assertCS(), assertWR(), assertRD(), A0Low(), A0High(), and writeDB()


    //write the byte
    writeDB(c);
    int i=0;
    for(i=0; i<1000; i++);
    //activate the transfer
    assertCS(1);

    //activate the write signal
    assertWR(1);

    //de-assert the write
    assertWR(0);

    //finish transfer
    assertCS(0);

}

// Send command to LCD.
static void writeCommand( uint8_t c ) {
  GPIO_PORTK[GPIO_DIR] = 0xff;  // prepare data bus for output
  A0Low();                      // prepare for command write
  writeLCD( c );
}

// Send data to LCD.
static void writeData( uint8_t c ) {
  GPIO_PORTK[GPIO_DIR] = 0xff;  // prepare data bus for output
  A0High();                     // prepare for data write
  writeLCD( c );
}

// Set page address.  This is our equivalent of setting the y location of 
// the cursor.
static void setPage( uint8_t p ) {
  writeCommand( ST7565R_PAGE_ADDRESS_SET | 
        ( ST7565R_PAGE_ADDRESS_SET_M & p ) );
}

// Set column address.  This is our equivalent of setting the x location of 
// the cursor, except it is a sub-character resolution.
static void setColumn( uint8_t a ) {
  writeCommand( ST7565R_COLUMN_ADDRESS_SET_HIGHBITS | 
        ( ST7565R_COLUMN_ADDRESS_SET_M & ( a >> 4 ) ) );
  writeCommand( ST7565R_COLUMN_ADDRESS_SET_LOWBITS |
        ( ST7565R_COLUMN_ADDRESS_SET_M & a ) );
}

// Delay t milliseconds
static void delayMS( uint32_t t ) {
  // Save current SysTick settings
  uint32_t ctrl = PPB[PPB_STCTRL];
  uint32_t reload = PPB[PPB_STRELOAD];

  // Configure to reload after 1 ms
  PPB[PPB_STCTRL] |= PPB_STCTRL_CLK_SRC | PPB_STCTRL_ENABLE;
  PPB[PPB_STRELOAD] = 120000000/1000-1;

  // Busy wait
  while( t-- )
    while( ( PPB[PPB_STCTRL] & PPB_STCTRL_COUNT ) == 0 );

  // Restore previous settings
  PPB[PPB_STCTRL] = ctrl;
  PPB[PPB_STRELOAD] = reload;
}

// --------------------------------------------
// Public procedures follow.
// --------------------------------------------

// Clear the LCD.
void clearLCD( void ) {
  uint8_t page, column;
  for( page = 0; page < LCD_HEIGHT / LINES_PER_PAGE; page++ ) {
    setPage( page );
    setColumn( 0 );
    for( column = 0; column < LCD_WIDTH; column++ ) {
      writeData( 0 ); // turn off all pixels
    }
  }
}

// Set address in character units.  0,0 is upper left corner.
void positionLCD( uint8_t y, uint8_t x ) {
  setPage( y );
  setColumn( x * CHAR_WIDTH );
}

// Display a NULL-terminated C string.  
void stringLCD(const char *str ) {
  while( *str != 0 ) {
    uint8_t i;
    const char *index = font[( *str - FIRST_CHAR )];
    writeData( 0 ); // write a blank column before each char
    for( i = 0; i < CHAR_WIDTH-1; ++i ) writeData( *index++ );
    ++str;
  }
}

// Initialize the GPIO ports and LCD itself.
void initLCD( void ) {
  // Enable RCGC clocks for Port F and Port K
  SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTF;
  SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTK;

  // Set all pins on Port K to inputs initially, then enable.

  GPIO_PORTK[GPIO_DIR] &= ~GPIO_ALL8;//set all DIR to 0 for input
  GPIO_PORTK[GPIO_DEN] |= GPIO_ALL8;//enable all pins

  // Enable all control pins on Port F and make them outputs.

  GPIO_PORTF[GPIO_DIR] |= GPIO_ALL8;//set all DIR to 1 for output
  GPIO_PORTF[GPIO_DEN] |= GPIO_ALL8;//enable all pins

  // Data sheet recommends reset be asserted for 20ms
  assertReset( true );
  delayMS( 20 );
  assertReset( false );

  // Deactivate all control pins
  assertCS( false );
  assertRD( false );
  assertWR( false );

  // Recommended initialization sequence.  The only change is the
  // argument for the Electronic Volume Mode Set; the recommended value
  // gave too low a contrast.

  const uint8_t init[] = { ST7565R_RESET,
                        ST7565R_LCD_BIAS_SET, 
                        ST7565R_ADC_SELECT_NORMAL,
                        ST7565R_COMMON_OUT_MODE_SELECT_REVERSE, 
                        ST7565R_DISPLAY_ON, 
                        ST7565R_DISPLAY_ALL_POINTS_OFF, 
                        ST7565R_DISPLAY_START_LINE_SET | 0, 
                        ST7565R_INTERNAL_RESISTOR_RATIO_SET | 5,
                        ST7565R_ELECTRONIC_VOLUME_MODE_SET, 48,
                        ST7565R_POWER_CONTROL_SET | 7,
                        ST7565R_READ_MODIFY_WRITE_END
                        };   
  const uint8_t *ptr = init;

  // ST7565R_READ_MODIFY_WRITE_END is use to mark the end of arguments
  // since it should not be used in the initialization.
  while( *ptr != ST7565R_READ_MODIFY_WRITE_END )
    writeCommand( *ptr++ );
}
