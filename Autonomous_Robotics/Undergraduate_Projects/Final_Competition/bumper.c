#include "bumper.h"

void initBumpers(){
//enable port A clock
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));

//set bumper pins as inputs
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_2);
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_3);

//(may or may not need pull ups)
    GPIOPadConfigSet(GPIO_PORTA_BASE,GPIO_PIN_2,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTA_BASE,GPIO_PIN_3,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);

    //setup interrupt for bumpers
    GPIOIntDisable(GPIO_PORTA_BASE, GPIO_PIN_2| GPIO_PIN_3);
    GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_2| GPIO_PIN_3);
    GPIOIntRegister(GPIO_PORTA_BASE, bumperHandler);
    GPIOIntTypeSet(GPIO_PORTA_BASE, GPIO_PIN_2| GPIO_PIN_3, GPIO_FALLING_EDGE | GPIO_RISING_EDGE);
    GPIOIntEnable(GPIO_PORTA_BASE, GPIO_PIN_2| GPIO_PIN_3);

    //master enable
    IntMasterEnable();
}

//set flag for the in-main de-bouncer code
void bumperHandler(){
    bumperTrigger=1;
    GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3);
}

//returns 0 for nothing, 1 for Right pushed, 2 for left pushed and 3 for both pushed
int getBumperStatus(){
    static uint32_t counterR=0;
    static uint32_t counterL=0;
    static uint32_t BumpStateL=0;
    static uint32_t BumpStateR=0;

            //de-bounce check bumpers when flag is set via interrupt handler
            if(bumperTrigger){
                for(int i=0; i<20; i++){
                    if((GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_2) & GPIO_PIN_2)==0)  counterR++;
                    if((GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_3) & GPIO_PIN_3)==0)  counterL++;
                }
                if(counterR>13){
                    BumpStateR=1;
                    counterR=0;
                }
                else    BumpStateR=0;
                if(counterL>13){
                        BumpStateL=1;
                        counterL=0;
                }
                else    BumpStateL=0;
                bumperTrigger=0;
            }
        if(BumpStateL==0&&BumpStateR==0)    return 0;
        else if(BumpStateL==1&&BumpStateR==0) return 1;
        else if(BumpStateL==0&&BumpStateR==1) return 2;
        else                                    return 3;
 }
