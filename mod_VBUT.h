#ifndef MOD_VBUT_H_
#define MOD_VBUT_H_

//
// mod_VBUT.h
// 
// Support for the Virtual Buttons for the ENCE361 Aviation Project
// Stellaris LM3S1968 EVK
// abw40
// Last Modified: 15.3.2015

// Initialise virtual buttons
void
initVBUT (void);

// Call to the SysCtlReset function if the soft reset virtual button is pressed
void
checkSoftReset (void);

// Control state function
long
Check_Control_State (long my_main_state);

// Poll Virtual Up/Down Buttons and update desired alt in 10% (0.08V) increments
long
desired_alt_update (long desired_alt);

// Poll Virtual CW/CCW buttons and update desired yaw in 15 degree increments
long
desired_yaw_update (long desired_yaw);

#endif /*MOD_VBUT_H_*/
