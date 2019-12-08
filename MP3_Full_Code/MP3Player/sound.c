// Updated thread and handler code for use with MP3 player

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "ssi_DAC.h"
#include "timer2A.h"

// Variables used by thread and handler to communicate
static volatile uint8_t queueLow, queueRefilled;
static volatile uint16_t dataLength, dataAdded;

static bool inNewChunk;
static uint8_t chunkingState;

// The size of a "chunk", based on what is passed from libmad.
static uint16_t chunkSize;

// The queue pointers for the sound data
static uint16_t *lbuffer = NULL;
static uint16_t *rbuffer = NULL;

// Pointers to head of sound data FIFOs
static uint16_t *lOutPtr,  *rOutPtr;

static uint8_t volume = 4;

// Read from pointers for number of bytes available.  When we have less 
// than a block of data remaining, inform main thread we need another block.
void writeDACs( void ) {
  // If main thread loaded more data, update our length counter.
  if( queueRefilled ) {
    dataLength += dataAdded;
    queueRefilled = 0;
  }

  // While there is more data, output it
  if( dataLength > 0 ) {

    // If we have less than a block, signal main thread
    if( dataLength <= chunkSize )
      queueLow = 1;

    // Send data to each DAC
    txDataSSI2( 0, *lOutPtr++ / volume );
    txDataSSI2( 1, *rOutPtr++ / volume );
    dataLength -= 1;

//  A       B       C       D       E
// |       |       |       |       |      |
// 1234567812345678123456781234567812345678
// 
// State 0: Load blocks A-D
// State 1: Start reading from A
//          wait until reading starts block B
//              inNewChunk = true;
//              chunk = 1;
// State 2: Move block D to block A
//          wait until reading starts block D
//              inNewChunk = true;
//              chunk = 3;
// State 3: Restart reading at block A
//          Load blocks B-E
//          wait until reading starts block B
//              inNewChunk = true;
//              chunk = 1;
// State 4: Move block E to Block A
//          wait until reading starts block E
//          goto Step 3
//              inNewChunk = true;
//              chunk = 0;
    {
    // check for a chunk change
      uint16_t offset = lOutPtr-lbuffer;
      if( offset % chunkSize == 0 ) {
        offset /= chunkSize;
        switch( chunkingState ) {
        case 1:
          if( offset == 1 ) {
            chunkingState = 2;
            inNewChunk = true;
          }
          break;
        case 2:
          if( offset == 3 ) {
            chunkingState = 3;
            lOutPtr = lbuffer;
            rOutPtr = rbuffer;
            inNewChunk = true;
          }
          break;
        case 3:
          if( offset == 1 ) {
            chunkingState = 4;
            inNewChunk = true;
          }
          break;
        case 4:
          if( offset == 4 ) {
            chunkingState = 3;
            lOutPtr = lbuffer;
            rOutPtr = rbuffer;
            inNewChunk = true;
          }
          break;
        }
        offset /= chunkSize;
      }
    }
  } else
    dataLength = 0;     // should only get here for end of file
  clearTimer2A();
}

// Initialize the variables use by the thread and handler.
void initSound( void ) {
  queueRefilled = 0;
  dataLength = dataAdded = 0;

  // Deallocate prior queues.
  free( lbuffer );
  free( rbuffer );
  lOutPtr = lbuffer = NULL;
  rOutPtr = rbuffer = NULL;

  // Set this so the first interrupt will request data.
  queueLow = 1;
  inNewChunk = true;
  chunkingState = 1;
}

// The updated thread code.  This is called when the MP3 decoder has more data
// to be played.  However, we must wait until the handler signals that it has 
// room in its queues for it.
void checkDACs( uint32_t *left, uint32_t *right, uint32_t len ) {
  // if paused, this is a good a place as any to twiddle our thumbs.
  do {
    __asm( " wfi" );
  } while( inNewChunk == false );

//  A       B       C       D       E
// |       |       |       |       |      |
// 1234567812345678123456781234567812345678
// 
// State 0: Load blocks A-D
// State 1: Start reading from A
//          wait until reading starts block B
//              inNewChunk = true;
//              chunk = 1;
// State 2: Move block D to block A
//          wait until reading starts block D
//              inNewChunk = true;
//              chunk = 3;
// State 3: Restart reading at block A
//          Load blocks B-E
//          wait until reading starts block B
//              inNewChunk = true;
//              chunk = 1;
// State 4: Move block E to Block A
//          wait until reading starts block E
//          goto Step 3
//              inNewChunk = true;
//              chunk = 0;

// State 0 and State 3 both do this

  // Save number of samples
  dataAdded = len;

  // If this is the first call, allocate the queues based on the
  // the length of the buffers being passed in.  This assumes that
  // the size is constant for the duration of this particular song.
  if( lbuffer == NULL ) {
    chunkSize = len / 4;
    lOutPtr = lbuffer = malloc ( 5 * chunkSize * sizeof( uint16_t ) );
    rOutPtr = rbuffer = malloc ( 5 * chunkSize * sizeof( uint16_t ) );
  }

  // Force everything into registers to speed up processing
  register uint32_t *l = left;
  register uint32_t *r = right;
  register uint16_t _len = len;
  register uint16_t *ldest = lbuffer;
  register uint16_t *rdest = rbuffer;

  if( chunkingState == 3 ) {
    ldest += chunkSize;
    rdest += chunkSize;
  }

  // Convert each 32-bit signed value into 12-bit unsigned values 
  // (since DAC is only 12 bits).
  while( _len-- ) {
    *ldest++ = ( ( (*l++) ^ 0x80000000 ) >> 20 );
    *rdest++ = ( ( (*r++) ^ 0x80000000 ) >> 20 );
  }

  // Now it's OK to set/clear flags
  queueRefilled = 1;
  queueLow = 0;

  if( chunkingState == 1 ) {
    inNewChunk = false;
    do {
      __asm( " wfi" );
    } while( inNewChunk == false );
    inNewChunk = false;
        // now copy block D to block A
    memcpy( lbuffer, lbuffer + chunkSize * 3, chunkSize * sizeof( uint16_t ) );
    memcpy( rbuffer, rbuffer + chunkSize * 3, chunkSize * sizeof( uint16_t ) );
  } else if( chunkingState == 3 ) {
    inNewChunk = false;
    do {
      asm( " wfi" );
    } while( inNewChunk == false );
    inNewChunk = false;
        // now copy block E to block A
    memcpy( lbuffer, lbuffer + chunkSize * 4, chunkSize * sizeof( uint16_t ) );
    memcpy( rbuffer, rbuffer + chunkSize * 4, chunkSize * sizeof( uint16_t ) );
  }
}

// Check whether we've sent everything to the DACs
bool isSoundFinished( void ) {
  return dataLength == 0;
}

// Future implementation: increase volume
void upVolume( void ) {
    int temp = volume-1;
    if(temp>0) volume = temp;
}

// Future implementation: decrease volume
void downVolume( void ) {
    int temp = volume+1;
    if(temp<11) volume = temp;
}

