#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "sysctl.h"
#include "gpio.h"
#include "qssi.h"
#include "ssi_uSD.h"

// Initialize and enable the QSSI0 module
void initSSI0( void ) {
    // Alternate function
    SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTA;
    SYSCTL[SYSCTL_RCGCSSI] |= SYSCTL_RCGCSSI_QSSI0;

    QSSI0[SSICR0] &= ~DSS;   //1 byte data size
    QSSI0[SSICR0] |= 0x7;
    QSSI0[SSICR0] &= ~SPO;     //clock edge low
    QSSI0[SSICR0] &= ~SPH;     //phase 1st clock

    // Enable the AFSEL on PA245
    GPIO_PORTA[GPIO_AFSEL] |= GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5;

    // Assign PA245 to QSSI0
    GPIO_PORTA[GPIO_PCTL] &= ~((0xf << (4*2)) | (0xf << (4*4)) | (0xf << (4*5)));
    GPIO_PORTA[GPIO_PCTL] |= (0xf << (4*2)) | (0xf << (4*4)) | (0xf << (4*5));

    // Set direction and enable
    GPIO_PORTA[GPIO_DEN] |= GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5;

    // Set the output for PA3
    GPIO_PORTA[GPIO_DIR] |= GPIO_PIN_3;
    GPIO_PORTA[GPIO_DEN] |= GPIO_PIN_3;
}

// Set the baud-rate divisor. The correct value is computed by dividing
// the prescaled clock rate by the desired baud rate.
void setSSI0FastSpeed( bool fast ) {
    //disable QSSI
    QSSI0[SSICR1] &= ~SSE;
    if(fast){
       // 12.5 MHz clock speed
       QSSI0[CPSDVR] = 0x04;//prescaler of 4
       QSSI0[SSICR0] &= ~SCR;
       QSSI0[SSICR0] |= (0x02 << 8);
       // enable QSSI0
       QSSI0[SSICR1] |= SSE;
    }
    else{
        QSSI0[CPSDVR] = 0x04;  // prescaler of 2
        QSSI0[SSICR0] &= ~SCR;
        QSSI0[SSICR0] |= (0x95 << 8);   // Decimal of 149
        // enable QSSI0
        QSSI0[SSICR1] |= SSE;
    }
}

// Send a single byte.
void txByteSSI0( uint8_t data ) {
    volatile uint8_t trash = 0;

    //while TXFIFO does not have space
    while ((QSSI0[QSSI_SR] & QSSI_SR_TNF) != QSSI_SR_TNF);
    //write data to FIFO
    QSSI0[QSSI_DR] = data;

    //while RXFIFO is empty
    while ((QSSI0[QSSI_SR] & QSSI_SR_RNE) != QSSI_SR_RNE);
    //read data and discard
    trash = QSSI0[QSSI_DR];
}

// Receive a single byte. You must write a 0xFF (the bus idles high) to
// receive the byte.
uint8_t rxByteSSI0( void ) {
    uint8_t data = 0;
    //while TXFIFO does not have space
    while ((QSSI0[QSSI_SR] & QSSI_SR_TNF) != QSSI_SR_TNF);
    //write to idle
    QSSI0[QSSI_DR] = 0xFF;

    //while RXFIFO is empty
    while ((QSSI0[QSSI_SR] & QSSI_SR_RNE) != QSSI_SR_RNE);
    //read data from FIFO
    data = QSSI0[QSSI_DR];
    return data;
}

void assertCS( bool level ) {
  // wait until SSI is not busy
  while( ( QSSI0[QSSI_SR] & QSSI_SR_BSY ) != 0 );

  if( level == true ) GPIO_PORTA[GPIO_PIN_3] = 0;// set pin low
  else GPIO_PORTA[GPIO_PIN_3] = GPIO_PIN_3;// set pin high
}
