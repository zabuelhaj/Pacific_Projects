/*
 * main.c
 *
 *  Created on: Apil 16, 2018
 *      Authors: zabuelhaj & crash
 */


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "inc/tm4c129cncpdt.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "inc/hw_types.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

/* Definitions: */

//P1 has a 1 second burst
#define P1_time 1
//P2 has a 2 second burst
#define P2_time 2
//P3 has a 3 second burst
#define P3_time 3
//constant used to multiply times into seconds on TIVA
#define mconst 100
//maximum processes to complete is 10 P1's
#define max_elements 10

#define max_wait 1000000

//global sw_flag:  1 - SW1 pressed, 2 - SW2 pressed, 3 - SW1 & SW2 are pressed
int sw_flag=0;

//process structure
struct process
{
    int PID;
    int waiting_time;
    int time_left;
};

volatile uint32_t millis = 0;

/*
  Interrupt handler for the timer
*/
void SysTickInt(void)
{
  uint32_t status=0;

  status = TimerIntStatus(TIMER5_BASE,true);
  TimerIntClear(TIMER5_BASE,status);
  millis++;
}

/*
  Timer setup
*/
void TimerBegin(){

  //We set the load value so the timer interrupts each 1ms
  uint32_t Period;
  Period = 80000; //1ms

  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
  SysCtlDelay(3);
  /*
    Configure the timer as periodic, by omission it's in count down mode.
    It counts from the load value to 0 and then resets back to the load value.
  REMEMBER: You need to configure the timer before setting the load and match
  */
  TimerConfigure(TIMER5_BASE, TIMER_CFG_PERIODIC);
  TimerLoadSet(TIMER5_BASE, TIMER_A, Period -1);

  TimerIntRegister(TIMER5_BASE, TIMER_A, SysTickInt);

  /*
    Enable the timeout interrupt. In count down mode it's when the timer reaches
  0 and resets back to load. In count up mode it's when the timer reaches load
  and resets back to 0.
  */
  TimerIntEnable(TIMER5_BASE, TIMER_TIMA_TIMEOUT);

  TimerEnable(TIMER5_BASE, TIMER_A);
}

/*
  This is the delay function.
*/
void Wait (uint32_t tempo) {
  volatile uint32_t temp = millis;
  while ( (millis-temp) < tempo);
}

int process_check(struct process process_array[max_elements]) {
    for(int i=0; i<max_elements; i++){
        //if something in PID then return true
        if (process_array[i].PID){
            return 1;
        }
    }
    return 0;
}

/* The interrupt handlers when reading timeout. */      // There is probably a more efficient way of handling this.
void intHandlerTimer0A (void) {
    /* Stuff to go here. */

    /* Clear the interrupt. */
    TimerIntClear (TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

/* Initialize all timer interrupts. */
void initInterruptTimer (void) {
    /* Disable timer. */
    TimerIntDisable (TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    /* Register handlers: */
    TimerIntRegister (TIMER0_BASE, TIMER_A, intHandlerTimer0A);

    /* Enable timer interrupt upon timeout. */
    TimerIntEnable (TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    /* Set necessary flags here. */
    // ...

    /* IntMasterEnable() called in main. */
}

/* Interrupt handler for PORTJ switches. */
void button_pressed (void) {
    int max_count = 100000;
    int counter1;
    int counter2;

    counter1 = 0;
    counter2=0;
    for(int i=0; i<max_count; i++){
        if (!GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0)) {
            counter1++;
        }
        if (!GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1)) {
            counter2++;
        }
    }

    if(counter1>(max_count/2)) counter1=-1;
    if(counter2>(max_count/2)) counter2=-1;

    //both pressed
    if ((counter1==-1) && (counter2==-1))sw_flag=3;
    //switch 1 pressed
    else if ((counter1==-1)&&(counter2!=-1))sw_flag=1;
    //switch 2 pressed
    else if ((counter1!=-1)&&(counter2==-1))sw_flag=2;
    //no switch pressed
//    else sw_flag=0;

    GPIOIntClear(GPIO_PORTJ_BASE, GPIO_LOW_LEVEL);
}

/* Initialize GPIO interrupts. */
void initInterruptGPIO (void) {

    //set SW to input on PJ0 & PJ1
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTJ_BASE,GPIO_PIN_0,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTJ_BASE,GPIO_PIN_1,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);

    //setup interupt for PJ0 & PJ1
    GPIOIntDisable(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOIntClear(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOIntRegister(GPIO_PORTJ_BASE, button_pressed);
    GPIOIntTypeSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_LOW_LEVEL);
    GPIOIntEnable(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /* IntMasterEnable() called in main. */
}

void processes_init(struct process process_array[max_elements]) {
    for(int i=0; i<max_elements; i++){
        process_array[i].PID = 0;
        process_array[i].waiting_time = 0;
        process_array[i].time_left = max_wait;
        }
}

int main(void){
    //build process array
    struct process P[max_elements];
    struct process temp_struct;
    int sort_flag=1;
    //int higher_flag=0;
    int last_empty_entry=9;
    int loop_ok=1;
    processes_init(P);

    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    TimerBegin();

/*
    //TEST CASE
    P[3].PID = 3;
    P[3].time_left = 5;
    P[3].waiting_time = 0;
    P[2].PID = 2;
    P[2].time_left = 6;
    P[2].waiting_time = 0;
    P[6].PID = 1;
    P[6].time_left = 3;
    P[6].waiting_time = 0;
*/
    /*
    should see it sort them in order, then you should see waiting times build up on 2 and 3 as 1 runs
    then p1 should dissapear and p3 should run and p2 should continue getting more wait time
    then p2 should run and finish
    */
    //END TEST CASE

    /* Set up the necessary peripherals for switches and timers. */
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOJ);
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_GPIOJ));
    GPIOPinTypeGPIOInput (GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);        // The switches to be used for process selection.
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOA);
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_GPIOA));
    GPIOPinTypeGPIOOutput (GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);     // LED output pins.
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOE);
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_GPIOE));
    GPIOPinTypeGPIOOutput (GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);       // LEDs.
    SysCtlPeripheralEnable (SYSCTL_PERIPH_TIMER0);
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_TIMER0));
    TimerConfigure (TIMER0_BASE, TIMER_CFG_A_ONE_SHOT_UP);

    /* Enable the interrupts for switches and timers. */
    initInterruptGPIO();
    initInterruptTimer();
    IntMasterEnable();

/*
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_PIN_2);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, GPIO_PIN_4);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, GPIO_PIN_5);
*/
    while(1){
        if (process_check(P)){
            //sort the array via bubble sort
            sort_flag=1;
            while(sort_flag){
                sort_flag=0;
                for(int i = 0; i<max_elements-1; i++){
                    if(P[i].time_left > P[i+1].time_left){
                        temp_struct = P[i];
                        P[i] = P[i+1];
                        P[i+1] = temp_struct;
                        sort_flag=1;
                    }
                }
            }
            //main while loop for servicing the process
            //while there is still time left in the process
            loop_ok=1;
            while(P[0].time_left && loop_ok){
                if(P[0].PID == 1) GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_PIN_2);
                else if(P[0].PID == 2) GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
                else GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, GPIO_PIN_4);
                //accrue wait time to all members in the queue aside from the first element since it is running
                for(int i=1; i<max_elements; i++){
                    //once you see an empty struct break since the rest of the array will be empty due to sorting
                    if(!P[i].PID){
                        last_empty_entry = i;
                        break;
                    }
                    //inrement waiting_time on all queued processes
                    P[i].waiting_time = P[i].waiting_time+1;
                }
                //check for a button press
                if(sw_flag){
                    //make the new process struct
                    temp_struct.PID = sw_flag;
                    switch(sw_flag){
                        case(1):
                        temp_struct.time_left = P1_time*mconst;
                        break;
                        case(2):
                        temp_struct.time_left = P2_time*mconst;
                        break;
                        case(3):
                        temp_struct.time_left = P3_time*mconst;
                        break;
                        default:
                        temp_struct.time_left = P1_time*mconst;
                        break;
                    }

                    temp_struct.waiting_time = 0;
                    //append to queue
                    P[last_empty_entry] = temp_struct;
                    sw_flag=0;
                    //if the new struct has less time left than the current one, break
                    if(temp_struct.time_left < P[0].time_left){
                        loop_ok = 0;
                        break;
                    }
                }

                Wait(10);
                //decrement process time left
                P[0].time_left = P[0].time_left-1;
                //remove process from array by setting PID to 0
                if(P[0].time_left==0){
                    P[0].PID=0;
                    P[0].time_left = max_wait;
                    P[0].waiting_time = 0;
                    break;
                }

            }
            GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, ~GPIO_PIN_2);
            GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, ~GPIO_PIN_3);
            GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, ~GPIO_PIN_4);
        }
            //append to queue if we see a switch push coming in
            if(sw_flag){
                //make the new process struct
                temp_struct.PID = sw_flag;
                switch(sw_flag){
                    case(1):
                    temp_struct.time_left = P1_time*mconst;
                    break;
                    case(2):
                    temp_struct.time_left = P2_time*mconst;
                    break;
                    case(3):
                    temp_struct.time_left = P3_time*mconst;
                    break;
                    default:
                    temp_struct.time_left = P1_time*mconst;
                    break;
                }
                temp_struct.waiting_time = 0;
                //append to queue
                P[last_empty_entry] = temp_struct;
                sw_flag=0;
            }

    }
}
