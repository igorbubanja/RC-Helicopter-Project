/*
 * main_control.c
 */

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

// Constants
//PWM
#define PWM_DIV_CODE SYSCTL_PWMDIV_4
#define PWM_DIVIDER 4
#define PWM4_RATE_HZ 150
#define PWM4_DEF_DUTY 50
#define PWM1_RATE_HZ 150
#define PWM1_DEF_DUTY 50

// Globals
//Yaw
volatile static unsigned long ulPinA; // Variables for Yaw Monitering
volatile static unsigned long ulPinB; // Changed by interrupts, hence volatile
volatile static unsigned long old_state;
volatile static unsigned long new_state;
volatile static long yaw = 0;
static long desired_yaw = 0;
//Alt
static unsigned long ADC_set_0[1];
static long alt = 0;
static long desired_alt = 0;
//PID
static long A_Kp = 2; // PID Control Constants
static long A_Ki = 2;
static long Y_Kp = 1;
static long Y_Ki = 1;
// Count Variables
static int takeoff_count = 0;
static int landing_count = 0;

//enumerate control states
enum  main_state {LANDED = 0, CONTROL, LANDING};
enum main_state my_main_state = LANDED;

// Yaw Interrupt Handler
void
PinIntHandler (void)
{
	// Clear the interrupt
	GPIOPinIntClear (GPIO_PORTF_BASE, GPIO_PIN_5 | GPIO_PIN_7);

	// Assign the previous new_state to the old_state variable
	old_state = new_state;

	// Read Pins
	ulPinA = GPIOPinRead (GPIO_PORTF_BASE, GPIO_PIN_5);
	ulPinB = GPIOPinRead (GPIO_PORTF_BASE, GPIO_PIN_7);

	// Assign new_state
	new_state = check_state (ulPinA, ulPinB);
	yaw += check_direction (old_state, new_state);
}

// Initialise Pins for Interrupt
void
initPins (void)
{
    // Enable and configure the port and pin used:  input on PF5: Pin 27
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOF);
    GPIOPadConfigSet (GPIO_PORTF_BASE, GPIO_PIN_5 | GPIO_PIN_7, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPU);
    // Register the handler for Port F into the vector table
    GPIOPortIntRegister (GPIO_PORTF_BASE, PinIntHandler);
    // Set pins as inputs
    GPIOPinTypeGPIOInput (GPIO_PORTF_BASE, GPIO_PIN_5 | GPIO_PIN_7);
    // Set up the pin change interrupt (both edges)
    GPIOIntTypeSet (GPIO_PORTF_BASE, GPIO_PIN_5 | GPIO_PIN_7, GPIO_BOTH_EDGES);
    // Enable the pin change interrupt
    GPIOPinIntEnable (GPIO_PORTF_BASE, GPIO_PIN_5 | GPIO_PIN_7);
    IntEnable (INT_GPIOF);  // Note: INT_GPIOF defined in inc/hw_ints.h
}

//initialise system clock
void
initClock(void){
	SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
	                   SYSCTL_XTAL_8MHZ);
}

// Main Control Task for the Heli-Rig
void
ControlTask (void)
{
	if (my_main_state == LANDED) { // Landed
		PWMOutputState (PWM_BASE, PWM_OUT_1_BIT, false); // Turn PWM's off
		PWMOutputState (PWM_BASE, PWM_OUT_4_BIT, false);
		desired_alt = 0;
		desired_yaw = 0;
		takeoff_count = 0;
		landing_count = 0;
	}
	else if (my_main_state == CONTROL) { // Takeoff and Manual Control
		PWMOutputState (PWM_BASE, PWM_OUT_1_BIT, true); // Enable PWM outputs
		PWMOutputState (PWM_BASE, PWM_OUT_4_BIT, true);
		
		if (takeoff_count > 5000) { // Disable manual control until after take-off sequence (10 seconds = 5000 counts)
			desired_alt = desired_alt_update(); // Poll buttons and update desired alt/yaw values
			desired_yaw = desired_yaw_update();
		}

		duty_tail_motor = Y_PID(Y_Kp, Y_Ki, desired_yaw, yaw); // Set motor duty's to control values
		duty_main_motor = A_PID(A_Kp, A_Ki, desired_alt, alt);

		if (desired_alt < 400 && takeoff_count < 5000) {
			desired_alt += 40 * takeoff_count / 500; // Increment desired altitude by 5% every second for a smooth take-off up to 50% altitude
		}
		takeoff_count++;

		setMotorPWM (duty_tail_motor, duty_main_motor, period); // Set PWM output to motors
	}
	else if (my_main_state == LANDING) { // Landing Control
		duty_tail_motor = Y_PID(Y_Kp, Y_Ki, desired_yaw, yaw); // Set motor duty's to control values
		duty_main_motor = A_PID(A_Kp, A_Ki, desired_alt, alt);

		if (duty_main_motor > 5) {
			desired_alt -= 40 * landing_count / 500; // Decrement desired altitude by 5% every second to smooth landing
		}

		setMotorPWM (duty_tail_motor, duty_main_motor, period); // Set PWM output to motors
		
		if (alt < 40) { // Turn PWM's off once the Heli altitude has reached a certain tolerance (40 = 5% max alt)
			PWMOutputState (PWM_BASE, PWM_OUT_1_BIT, false);
			PWMOutputState (PWM_BASE, PWM_OUT_4_BIT, false);
		}
		landing_count++;
	}
}


// Main Round-Robin Loop
int main(void) {
	unsigned long period;
	unsigned int duty_tail_motor = PWM4_DEF_DUTY;
	unsigned int duty_main_motor = PWM1_DEF_DUTY;

	// Initialisations
	initVBUT();
	initClock();
	initRefPin();
	initPins();
	initADC();
	initPWM();

	ulPinA = GPIOPinRead (GPIO_PORTF_BASE, GPIO_PIN_5); // Read initial Pin Values
	ulPinB = GPIOPinRead (GPIO_PORTF_BASE, GPIO_PIN_7);
	
	new_state = check_state(ulPinA, ulPinB); // Get Initial Yaw State

	ADCSequenceDataGet(ADC0_BASE, 3, ADC_set_0); // Reads initial ADC value as 'zero' height val

	//PWM setup
	period = SysCtlClockGet() / PWM_DIVIDER / PWM4_RATE_HZ; // Calculate PWM period
	setMotorPWM(duty_tail_motor, duty_main_motor) // Set PWM duty outputs to default values

	//Enable Interrupts
	IntMasterEnable();
	
	while(1) {
		checkSoftReset(); // Poll the soft reset button and call SysCtlReset if required
		alt_read(ADC_set_0[0]); // Reads the alt value from ADC0, returns averaged value over 20 cycles
		my_main_state = Check_Control_State(my_main_state); // Updates control state 
		
		ControlTask(); // Runs main control task
		
		SysCtlDelay(SysCtlClockGet() / (1500)); // Delay loop polling period (2ms)
	}
}
