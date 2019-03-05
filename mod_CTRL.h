#ifndef MOD_CTRL_H_
#define MOD_CTRL_H_

//
// mod_CTRL.h
// 
// Support for PID control over the Yaw and Alt of the Heli-Rig for the ENCE361 Aviation Project
// Stellaris LM3S1968 EVK
// abw40
// Last Modified: 15.3.2015

// Calculate from Altitude error PID control PWM Output
long
A_PID (long A_Kp, long A_Ki, long desired_alt, long alt);

// Calculate from Yaw error PID control PWM Output
long
Y_PID (long Y_Kp, long Y_Ki, long desired_yaw, long yaw);

#endif /*PID_YAW_H_*/
