//
// mod_CTRL.c
// 
// Support for PID control over the Yaw and Alt of the Heli-Rig for the ENCE361 Aviation Project
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

// Globals
static long delta_t = 2;
static long YI_error = 0;
static long AI_error = 0;


// Calculate from Altitude error PID control PWM Output
long
A_PID (long A_Kp, long A_Ki, long desired_alt, long alt) 
{
    long A_error = (desired_alt - alt)/8; // Calculate percentage error (-100% to +100%)
    
    AI_error += delta_t*A_error; // Cumalative integral error sum
    A_control = (A_Kp*A_error + A_Ki*AI_error/1000); // Output control PWM

    if (A_control > 65) { // Limit maximum PWM output to 65
        A_control = 65;
    }
    else if (A_control < 5) { // Limit minimum PWM output to 5
        A_control = 5;
    }

    return A_control;
}

// Calculate from Yaw error PID control PWM Output
long
Y_PID (long Y_Kp, long Y_Ki, long desired_yaw, long yaw)
{
    long Y_error = ((yaw*360/448 - desired_yaw)*100)/360; // Calculate percentage error (-100% to 100%)
    
    YI_error += delta_t*Y_error; // Cumalative integral error sum
    Y_control = (Y_Kp*Y_error + Y_Ki*YI_error/1000); // Output control PWM
    if (Y_control > 95) { // Limit maximum PWM output to 95
        Y_control = 95;
    }
    if (Y_control < 5) { // Limit minimum PWM output to 5
        Y_control = 5;
    }

    return Y_control;
}