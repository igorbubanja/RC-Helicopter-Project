//
// mod_PWM.c
// 
// PWM Support for the ENCE361 Aviation Project
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

// Constants
// PWM
#define PWM_DIV_CODE SYSCTL_PWMDIV_4
#define PWM4_RATE_HZ 150
#define PWM_DIVIDER 4
#define PWM4_DEF_DUTY 50
#define PWM1_DEF_DUTY 50
#define PWM1_RATE_HZ 150

// Initialise PWM
void
initPWM (void)
{
	unsigned long period1;
	unsigned long period2;

    // Initialise PWM Peripheral on PWM4, PF2 (Pin 22)
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOF);
    GPIOPinTypePWM (GPIO_PORTF_BASE, GPIO_PIN_2);

    // Initialise PWM Peripheral on PWM1, PD1 (Pin 53)
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOD);
    GPIOPinTypePWM (GPIO_PORTD_BASE, GPIO_PIN_1);

    SysCtlPeripheralEnable (SYSCTL_PERIPH_PWM);
    //
    // Compute the PWM period based on the system clock.
    //
        SysCtlPWMClockSet (PWM_DIV_CODE);
    PWMGenConfigure (PWM_BASE, PWM_GEN_2,
        PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenConfigure (PWM_BASE, PWM_GEN_0,
        PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    period1 = SysCtlClockGet () / PWM_DIVIDER / PWM1_RATE_HZ;
    period2 = SysCtlClockGet () / PWM_DIVIDER / PWM4_RATE_HZ;
    PWMGenPeriodSet (PWM_BASE, PWM_GEN_0, period1);
    PWMGenPeriodSet (PWM_BASE, PWM_GEN_2, period2);
    PWMPulseWidthSet (PWM_BASE, PWM_OUT_1, period1 * PWM1_DEF_DUTY / 100);
    PWMPulseWidthSet (PWM_BASE, PWM_OUT_4, period2 * PWM4_DEF_DUTY / 100);
    //
    // Enable the PWM output signal.
    //
    PWMOutputState (PWM_BASE, PWM_OUT_1_BIT, true);
    PWMOutputState (PWM_BASE, PWM_OUT_4_BIT, true);
    //
    // Enable the PWM generator.
    //
    PWMGenEnable (PWM_BASE, PWM_GEN_0);
    PWMGenEnable (PWM_BASE, PWM_GEN_2);
}

//Set PWM for tail and motor based on input duty cycles
void
setMotorPWM (unsigned long duty_tail_motor, unsigned long duty_main_motor, unsigned long period)
{
	PWMPulseWidthSet (PWM_BASE, PWM_OUT_4, period * duty_tail_motor /100);
	PWMPulseWidthSet (PWM_BASE, PWM_OUT_1, period * duty_main_motor /100);
}

