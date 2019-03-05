#ifndef MOD_YAWM_H_
#define MOD_YAWM_H_

//
// mod_YAWM.h
// 
// Yaw Monitering Support for the ENCE361 Aviation Project
// Stellaris LM3S1968 EVK
// abw40
// Last Modified: 15.3.2015

//initialise reference pin
void
initRefPin (void);

// Check Yaw direction
long
check_direction (unsigned long old_state, unsigned long new_state);

// Check Quadrature State
unsigned long
check_state (unsigned long ulPinA, unsigned long ulPinB);

#endif /*MOD_YAWM_H_*/
