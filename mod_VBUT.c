//
// mod_VBUT.c
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

// Initialise virtual buttons
void
initVBUT (void)
{
	SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOB);
	GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet (GPIO_PORTB_BASE, (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6) , GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

// Call to the SysCtlReset function if the soft reset virtual button is pressed
void
checkSoftReset (void)
{
	tBoolean vbutton_reset = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1);
	if (!vbutton_reset) {
		SysCtlReset();
	}
}

// Control state function
long
Check_Control_State (long my_main_state)
{
	tBoolean vselect = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_4);

	if (!vselect) { // Select button cycles through states 1->3
		my_main_state += 1;
		if (my_main_state > 3) {
			my_main_state = 0;
		}
	}
	return my_main_state;
}

// Poll Virtual Up/Down Buttons and update desired alt in 10% (0.08V) increments
long
desired_alt_update (long desired_alt)
{
	tBoolean vbutton_up = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_5); // reads button up pin
	tBoolean vbutton_down = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_6); //
	if (!vbutton_up && desired_alt < 800) {
		desired_alt += 80;
	}
	else if (!vbutton_down && desired_alt > 0) {
		desired_alt -= 80;
	}
	if (desired_alt > 800) {
		desired_alt = 800;
	}
	if (desired_alt < 0) {
		desired_alt = 0;
	}
	return desired_alt;
}

// Poll Virtual CW/CCW buttons and update desired yaw in 15 degree increments
long
desired_yaw_update (long desired_yaw)
{
	tBoolean vbutton_cw = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_2);
	tBoolean vbutton_ccw = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_3);
	if (!vbutton_ccw) {
		desired_yaw += 15;
	}
	else if (!vbutton_cw) {
		desired_yaw -= 15;
	}
	if (desired_yaw > 180) {
		desired_yaw = 180;
	}
	if (desired_yaw < -180 ) {
		desired_yaw = -180;
	}
	return desired_yaw;
}



