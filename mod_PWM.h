#ifndef MOD_PWM_H_
#define MOD_PWM_H_

//
// mod_PWM.h
// 
// PWM Support for the ENCE361 Aviation Project
// Stellaris LM3S1968 EVK
// abw40
// Last Modified: 15.3.2015

// Initialise PWM
void
initPWM (void);

//Set PWM for tail and motor based on input duty cycles
void
setMotorPWM (unsigned long duty_tail_motor, unsigned long duty_main_motor, 
				unsigned long period);


#endif /*MOD_PWM_H_*/
