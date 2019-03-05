#ifndef MOD_ADC_H_
#define MOD_ADC_H_

//
// mod_ADC.h
// 
// Support for the Virtual Buttons for the ENCE361 Aviation Project
// Stellaris LM3S1968 EVK
// abw40
// Last Modified: 15.3.2015

// Initialise the ADC0 Peripheral
void
initADC (void);

// Reads Altitude and assigns it to static alt variable (value between 0 and 800 --> 0 and 0.8V)
long
alt_read (unsigned long ADC_zero_val);

#endif /*MOD_ADC_H_*/
