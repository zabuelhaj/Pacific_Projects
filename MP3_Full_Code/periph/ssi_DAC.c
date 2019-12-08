#include <stdint.h>
#include <stdbool.h>

#include "sysctl.h"
#include "gpio.h"
#include "qssi.h"

#include "ssi_DAC.h"

// Initialize and enable the SPI module on PortD.  The Clk, FSS, and Tx pins are needed.
// The clock should be initialized to the highest speed.
void initSSI2( void ) {
    /* Set the clock for GPIO_PORTD and for QSSI2. */
    SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTD;
    SYSCTL[SYSCTL_RCGCSSI] |= SYSCTL_RCGCSSI_QSSI2;

    /* Disable QSSI2 before configuring it. */
    QSSI2[SSICR1] &= ~SSE;

    QSSI2[SSICR0] &= ~DSS;      // 2 byte data size.
    QSSI2[SSICR0] |= 0xF;
    QSSI2[SSICR0] &= ~SPO;      // Clock edge low.
    QSSI2[SSICR0] &= ~SPH;      // Phase 1st clock.
    QSSI2[CPSDVR] = 0x4;//prescaler of 4

    /* Enable the AFSEL on PD321. */
    GPIO_PORTD[GPIO_AFSEL] |= GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;

    /* Assign PD321 to QSSI2. */
    GPIO_PORTD[GPIO_PCTL] &= ~((0xf << (4*3)) | (0xf << (4*2)) | (0xf << (4*1)));
    GPIO_PORTD[GPIO_PCTL] |= (0xf << (4*3)) | (0xf << (4*2)) | (0xf << (4*1));

    /* Set direction and enable. */
    GPIO_PORTD[GPIO_DEN] |= GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;

    /* Enable QSSI2. */
    QSSI2[SSICR1] |= SSE;
}

// Send a half-word over the SPI port to the DAC.  The DAC is only 12 bits of resolution,
// but the remaining 4 bits must be set properly.  Since Rx is unused, you can ignore the
// Rx FIFO.
void txDataSSI2( uint8_t dac, uint16_t data ) {
    volatile uint8_t trash = 0;

    /* While TXFIFO does not have space. */
    while ((QSSI2[QSSI_SR] & QSSI_SR_TNF) != QSSI_SR_TNF);

    // Check data sheet for the dac, page 24
    // ignore shutdown. Add in to the buffer and add in the bits for which adc

    /* Write data to FIFO. */
    QSSI2[QSSI_DR] = (dac<<15) | (0<<14) | (1<<13) | (1<<12) | data;

    /* Take out the trash. */
    trash = QSSI2[QSSI_DR];
}
