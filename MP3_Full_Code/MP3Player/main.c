/*
 * main.c
 *
 *  Created on: Oct 6, 2017
 *      Author: zabuelhaj
 */

#include <stdint.h>
#include <stdbool.h>

// Peripherals
#include "osc.h"
#include "ssi_uSD.h"
#include "ssi_DAC.h"
#include "timer2A.h"
#include "lcd.h"

// FatFs
#include "ff.h"

// Utilities
#include "fileutils.h"
#include "ID3.h"

// Other project files
#include "control.h"
#include "UI.h"
#include "mp3.h"
#include "sound.h"

//keypad business...
#include "keypad.h"

//ABT type
const uint8_t lookup[4][4] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'},
};

// Decode and play an MP3 file.
void playSong( FIL *fp ) {
  // Clear the display
  clearLCD ();

  // Process ID3 header (if any).
  getID3Tags( fp , &tags );

  // print info
  positionLCD ( 0, 0 );
  tags.title[40] = 0;
  stringLCD ( (char*)tags.title );

  positionLCD ( 1, 0 );
  tags.artist[40] = 0;
  stringLCD ( (char*)tags.artist);

  positionLCD ( 2, 0 );
  tags.album[40] = 0;
  stringLCD ( (char*)tags.album );

  // Prepare for sound output.
  initSound();

   // Decode and play the song.
  MP3decode( fp );

   // Wait for the last data to be sent to the DACs.
  while( isSoundFinished() == false ) __asm( " wfi" );

  // Stop the DAC timer.
  enableTimer2A( false );
}

main() {
  // Initialize clock, SSIs, Timer, and LCD
  initOsc();
  initSSI0();
  initSSI2();
  initTimer2A();
  initLCD();
  initUI();
  //init the keypad with the lookup table
  initKeypad(lookup);

  // Initialize structure.
  FATFS fs; f_mount( &fs, "", 0 );

  // Find out how many MP3 files are on the card.
  uint8_t numSongs = countMP3();
  setNumSongs( numSongs );

  // Obligatory endless loop.
  while( true ) {
    uint8_t song;
    FIL fp;

    // Get the next file from the MicroSD card.
    song = getSong();
    findMP3( song, &fp );

    // Send the file to the MP3 decoder
    playSong( &fp );
  }
}
