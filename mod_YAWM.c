//
// mod_YAWM.c
// 
// Yaw Monitering Support for the ENCE361 Aviation Project
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

//initialise reference pin
void
initRefPin (void)
{
   // To set Pin 56 (PD0) as a +Vcc low current capacity source:
   SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOD);
   GPIOPinTypeGPIOOutput (GPIO_PORTD_BASE, GPIO_PIN_0);
   GPIOPadConfigSet (GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_STRENGTH_8MA,
      GPIO_PIN_TYPE_STD_WPU);
   GPIOPinWrite (GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_PIN_0);
}

// Check Yaw direction
long
check_direction (unsigned long old_state, unsigned long new_state)
{
    if ((new_state == old_state + 1) || (new_state == 0 & old_state == 3)) {
        return -1;
    }
    else if ((new_state == old_state - 1) || (new_state == 3 & old_state == 0)) {
        return 1;
    }
}

// Check Quadrature State
unsigned long
check_state (unsigned long ulPinA, unsigned long ulPinB)
{long
    unsigned long state;
    if (ulPinA == 0 & ulPinB == 0) {
        state = 0;
    }
    else if (ulPinA == 0 & ulPinB != 0) {
        state = 1;
    }
    else if (ulPinA != 0 & ulPinB != 0) {
        state = 2;
    }
    else if (ulPinA != 0 & ulPinB == 0) {
        state = 3;
    }
    return state;
}
