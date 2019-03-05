//
// mod_ADC.c
// 
// Support for the Virtual Buttons for the ENCE361 Aviation Project
// Stellaris LM3S1968 EVK
// abw40
// Last Modified: 15.3.2015

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "drivers/rit128x96x4.h"
#include "stdio.h"
#include "stdlib.h"
#include "driverlib/uart.h"

// Initialise the ADC0 Peripheral
void
initADC (void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
	                             ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 3);
	ADCIntClear(ADC0_BASE, 3);
}

// Reads Altitude and assigns it to static alt variable (value between 0 and 800 --> 0 and 0.8V)
long
alt_read (unsigned long ADC_zero_val)
{
	static int windex = 0;
	static int rindex = 0;
	static unsigned long ulValue[1];
	static unsigned long alt_array[20];
	long alt = 0;
	unsigned long sum = 0;

	ADCProcessorTrigger(ADC0_BASE, 3);
	while(!ADCIntStatus(ADC0_BASE, 3, false))
	        {
	        }
	ADCSequenceDataGet(ADC0_BASE, 3, ulValue);
	alt_array[windex] = ADC_zero_val - ulValue[0];
	windex ++;
	if (windex > 19) {
		windex = 0;
	}
	for (rindex = 0; rindex < 20; rindex++) {
		sum = sum + alt_array[rindex];
	}
	alt = sum*3000/1023/20;
	if (alt > 50000) {
		alt = 0;
	}

	return alt;
}



