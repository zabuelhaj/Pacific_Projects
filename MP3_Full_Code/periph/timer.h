// boilerplate for timer control

#ifndef __TIMER_H__
#define __TIMER_H__

// Configure the timer to roll over at a 200Hz frequency.
void initTimer( void );

// Wait for the timer to roll over, then return.
void waitOnTimer( void );

#endif // __TIMER_H__
