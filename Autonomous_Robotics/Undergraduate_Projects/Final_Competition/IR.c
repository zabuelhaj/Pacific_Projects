#include "IR.h"

//elements are stored for pin E3-E1 in puiVal index 0-2 respectively
uint32_t pui32ADC0Value[3];
const float volt_per_step = 0.00080566406;

void initADC(){
    //SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_ADC0));
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_GPIOE));

    //initially its pins 2 and 3
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1);
    ADCSequenceDisable(ADC0_BASE, 0);
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC0_BASE,0, 2, ADC_CTL_CH2 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 0);
}

//get data from ADC pins
void pullADC(){
    ADCIntClear(ADC0_BASE, 0);
    ADCProcessorTrigger(ADC0_BASE, 0);
    while(!ADCIntStatus(ADC0_BASE, 0, false));
    ADCSequenceDataGet(ADC0_BASE, 0, pui32ADC0Value);
}

//returns distance in cm
void getIRDistance(uint32_t * Dist){
    float V[]={0,0,0};

    //get sum of values over 5 runs
    for(int i=0; i<5; i++){
        //update ADC values
        pullADC();
        //get voltages on each pin E1-E3 with V[0]-V[2] respectively
        V[0] = V[0] + pui32ADC0Value[2]*volt_per_step;
        V[1] = V[1] + pui32ADC0Value[1]*volt_per_step;
        V[2] = V[2] + pui32ADC0Value[0]*volt_per_step;
        //small delay for ADC
        for(int i=0; i<20; i++);
    }

    //now take average
    V[0] = V[0]/5;
    V[1] = V[1]/5;
    V[2] = V[2]/5;

    //calculate distances based on measured voltages
    Dist[0] = (uint32_t)5.591805 + 75.14265*exp(-2.478459*V[0]);
    Dist[1] = (uint32_t)5.591805 + 75.14265*exp(-2.478459*V[1]);
    Dist[2] = (uint32_t)5.591805 + 75.14265*exp(-2.478459*V[2]);

}





