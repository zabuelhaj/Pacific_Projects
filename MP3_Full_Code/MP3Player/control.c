/*
 * control.c
 *
 *  Created on: Oct 7, 2017
 *      Author: zabuelhaj
 */

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "control.h"
#include <stdio.h>
#include <stdlib.h>

static uint8_t iter;
static uint8_t songBuffer[10];
// The total number of songs on the MicroSD card.
static uint8_t numSongs;
// The next song to play.
static uint8_t song = 0;
// Indicates termination of current song by user.
static bool done = false;
// Indicates whether decoding/playing is playing or paused.
static bool paused = false;
// Indicates if the MP3 player is in song shuffle mode.
static bool shuffle = false;

// Private procedure for selecting the next song in shuffle mode.
static uint8_t getShuffle( uint8_t song ) {
  srand(time(NULL));
  song = rand() % (numSongs + 1 - 1) + 1;
  return song;
}

// Return the number of the song to play.  Initially, just
// return the next sequential song, and wrap back to 0 when all
// songs have been returned.
uint8_t getSong( void ) {
  // Save the song number for the end.
  uint8_t tmp = song;

  // Pick the next song to play.
  if( shuffle == false ) {
    song = ( song + 1 ) % numSongs;
  } else {
    song = getShuffle( song );
  }
//reset buffer left by 1
  if(iter==9){
      for (int k = 9; k >= 0; k--){
          songBuffer[k]=songBuffer[k-1];
      }
      iter=8;
  }
  if(songBuffer[iter]!=tmp){
      iter+=1;
      songBuffer[iter]=tmp;
  }

  // Return song number.
  return tmp;
}

uint8_t getLast(void){
    if(iter==0) return getCurrentSong();
    else{
    songBuffer[iter]=0;
    iter-=1;
    return songBuffer[iter];
    }
}

// Store the total number of songs (needed by getSong()).
void setNumSongs( uint8_t num ) {
  numSongs = num;
}

uint8_t getCurrentSong(void){
    return song;
}

// Store the total number of songs (needed by getSong()).
uint8_t getNumSongs( void ) {
  return numSongs;
}

// Set next song explicitly.
void setSong( uint8_t v ) {
  song = v;
}

// Indicates whether the current MP3 song should be terminated early.
bool isDone( void ) {
  bool tmp = done;
  done = false;
  return tmp;
}

// Set song to terminate.
void setDone( void ) {
  done = true;
}

// Indicates whether the MP3 player is paused.
bool isPaused( void ) {
  return paused;
}

// Set state of pause.
void setPaused( bool v ) {
  paused = v;
}

// Indicates state of shuffle mode.
bool isShuffle( void ) {
  return shuffle;
}

// Set state of shuffle mode.
void setShuffle( bool v ) {
  shuffle = v;
}

uint16_t getFirstDigit(uint16_t key){
    return 10*(key-48);
}


