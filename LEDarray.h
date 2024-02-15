#ifndef _LEDarray_H
#define _LEDarray_H

#include <xc.h>

#define _XTAL_FREQ 64000000

//function prototypes
void LEDarray_init(void);
void LEDarray_disp_bin(unsigned int number);
void LEDarray_disp_PPM(unsigned int number, unsigned int max);
unsigned int calc_max_PPM(unsigned int cur_val, unsigned int max_ppm);
unsigned int LED_Light_Meter(unsigned int max_light, unsigned int min_light, unsigned int light_value);
unsigned int highestBit(unsigned int value);

#endif
