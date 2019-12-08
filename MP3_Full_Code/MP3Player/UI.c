/*
 * UI.c
 *
 *  Created on: Oct 7, 2017
 *      Author: zabuelhaj
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "gptm.h"
#include "gpio.h"
#include "ppb.h"
#include "sysctl.h"
#include "keypad.h"
#include "sound.h"

#include "control.h"

// Return key value upon initial key press.  Since keys are
// ASCII, they cannot have a value greater than 0xff, so when
// no new key is pressed the procedure returns UINT16_MAX.
static uint16_t UIKey( void ) { 
  static enum {NOTPRESSED, PRESSED} pressState = NOTPRESSED;
  uint8_t key;


  if (getKey(&key)) pressState = PRESSED;
  else pressState = NOTPRESSED;

  switch( pressState ) {
  case NOTPRESSED:
      return UINT16_MAX;
  case PRESSED:
    return (uint16_t) key;
  }
  return UINT16_MAX;
}

enum keycmds_t {
  PLAY_PAUSE    = 'A',
  SHUFFLE       = 'B',
  VOLUME_UP     = 'C',
  VOLUME_DOWN   = 'D',
  SKIP_BACKWARD = '*',
  SKIP_FORWARD  = '#',
};

// Interrupt handler for user interface.  When called it scans the
// keypad and if a new key is detected, performs necessary actions.
void UIHandler( void ) {
  uint16_t key = UIKey( ); 
  static uint16_t lastKeyValue;
  static uint16_t tempFirst;
  static bool flag_num;
  if( key != UINT16_MAX ) {
    if (lastKeyValue != key){
        switch( (enum keycmds_t)key ) {
        case PLAY_PAUSE:    // 'A'
          //if A then pause
          if(isPaused())  setPaused(false);
          else            setPaused(true);
          break;
        case SHUFFLE:       // 'B'
            if(isShuffle())  setShuffle(false);
            else            setShuffle(true);
          break;
        case VOLUME_UP:     // 'C'
            upVolume();
          break;
        case VOLUME_DOWN:   // 'D'
            downVolume();
          break;
        case SKIP_BACKWARD: // '*'
            setSong(getLast());
            setDone();
          break;
        case SKIP_FORWARD:  // '#'
            setDone();
          break;
        default:            // Numeric keys
            if(isPaused()&&!flag_num){
                tempFirst = 10*(key-48);
                flag_num=true;
            }
            else if(isPaused()&&flag_num){
                uint16_t tmp_full = tempFirst + (key-48);
                flag_num=false;
                setSong(tmp_full);
                setDone();
            }
            else{
                flag_num=false;
            }
          break;
        }
    }
  }
  lastKeyValue = key;

  // Clear the time-out.
  GPTM_TIMER3[GPTM_ICR] |= GPTM_ICR_TATOCINT;
}

// #define KEYPAD_PHONE
// #define KEYPAD_ABT

static const uint8_t lookup[4][4] = {
#ifdef KEYPAD_PHONE
  {'1',           '2', '3',          PLAY_PAUSE},
  {'4',           '5', '6',          SHUFFLE},
  {'7',           '8', '9',          VOLUME_UP},
  {SKIP_BACKWARD, '0', SKIP_FORWARD, VOLUME_DOWN},
#endif
#ifdef KEYPAD_ABT
  {'0',           '1',          '2',         '3'},
  {'4',           '5',          '6',         '7'},
  {'8',           '9',          VOLUME_UP,   SHUFFLE},
  {SKIP_BACKWARD, SKIP_FORWARD, VOLUME_DOWN, PLAY_PAUSE},
#endif
};

void initUI( void ) {
  // Prep the keypad.
  initKeypad( lookup );

  // Enable Run Clock Gate Control
  SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGCTIMER_TIMER3;
  SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGCTIMER_TIMER3;

  // 32-bit periodic timer.
  GPTM_TIMER3[GPTM_CFG] &= ~GPTM_CFG_M;
  GPTM_TIMER3[GPTM_CFG] |= GPTM_CFG_32BITTIMER;
  GPTM_TIMER3[GPTM_TAMR] &= ~GPTM_TAMR_TAMR_M;
  GPTM_TIMER3[GPTM_TAMR] |= GPTM_TAMR_TAMR_PERIODIC;

  // Set reload value for 20Hz
  GPTM_TIMER3[GPTM_TAILR] = (120000000/20)-1;

  // Enable the interrupt.
  GPTM_TIMER3[GPTM_IMR] |= GPTM_IMR_TATOIM;

  // Enable interrupt for timer.
  PPB[PPB_EN1] |= PPB_EN1_TIMER3A;

  // Set priority level to 1 (lower priority than Timer2A).
  PPB[PPB_PRIx] = ( PPB[PPB_PRIx] & ~PPB_PRI_INTy_M ) | (1 << PPB_PRI_INTy_S);

  // Clear the time-out.
  GPTM_TIMER3[GPTM_ICR] |= GPTM_ICR_TATOCINT;

  // Enable the timer.
  GPTM_TIMER3[GPTM_CTL] |= GPTM_CTL_TAEN | GPTM_CTL_TASTALL;
}
