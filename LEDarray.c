#include <xc.h>
#include "LEDarray.h"

/************************************
/ LEDarray_init
/ Function used to initialise pins to drive the LEDarray
************************************/
void LEDarray_init(void)
{
    LATA = 0; //set initial output LAT values (they may have random values when powered on)
    LATB = 0; //initialise LAT before TRIS
    LATFbits.LATF0 = 0;
    LATFbits.LATF6 = 0;
    LATG = 0;
	
    TRISA = 0; //set up TRIS registers for pins connected to LED array
	TRISB = 0;
    TRISFbits.TRISF0 = 0;
    TRISFbits.TRISF6 = 0;
    TRISG = 0;
    
    TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin    
}

/************************************
/ LEDarray_disp_bin
/ Function used to display a number on the LED array in binary
************************************/
void LEDarray_disp_bin(unsigned int number)
{
    //some code to turn on/off the pins connected to the LED array
	if (number & 1)        {LATGbits.LATG0 = 1;} else {LATGbits.LATG0 = 0;} //test bit 0 against bit mask and set LED
    if (number & (1 << 1)) {LATGbits.LATG1 = 1;} else {LATGbits.LATG1 = 0;} //test bit 1
    if (number & (1 << 2)) {LATAbits.LATA2 = 1;} else {LATAbits.LATA2 = 0;} //test bit 2
    if (number & (1 << 3)) {LATFbits.LATF6 = 1;} else {LATFbits.LATF6 = 0;} //...
    if (number & (1 << 4)) {LATAbits.LATA4 = 1;} else {LATAbits.LATA4 = 0;}
    if (number & (1 << 5)) {LATAbits.LATA5 = 1;} else {LATAbits.LATA5 = 0;}
    if (number & (1 << 6)) {LATFbits.LATF0 = 1;} else {LATFbits.LATF0 = 0;}
    if (number & (1 << 7)) {LATBbits.LATB0 = 1;} else {LATBbits.LATB0 = 0;}
    if (number & (1 << 8)) {LATBbits.LATB1 = 1;} else {LATBbits.LATB1 = 0;} //test bit 8
}

/************************************
/ LEDarray_PPM
/ Function used to combine and display the current light value and peak hold
/cur_val is the current light reading, max_ppm is the peak hold
************************************/
void LEDarray_disp_PPM(unsigned int cur_val, unsigned int max_ppm)
{
    // Combine meter_value with max_ppm using bitwise OR
    cur_val |= max_ppm ;
        
    //output the value to the LED array
    LEDarray_disp_bin(cur_val);
}

/************************************
/ calc_max_PPM
/ Function used to calculate the peak hold
/ cur_val is the current level from the most recent sample, and max_ppm is the peak value for the last second
************************************/
unsigned int calc_max_PPM(unsigned int cur_val, unsigned int max_ppm) {
    static unsigned long loopCounter = 0;
    const unsigned long threshold = 10000; // Adjust this based on your loop speed

    // Update max_ppm if cur_val is greater
    if (cur_val > max_ppm) {
        max_ppm = cur_val;

        // Find and store only the MSB of max_ppm
        max_ppm = highestBit(max_ppm);
        
        loopCounter = 0;
    }
    else {
        // Increment loop counter and check if threshold is reached
        loopCounter++;

        if (loopCounter >= threshold) {
            max_ppm >>= 1; // Shift max_ppm value by one
            loopCounter = 0; // Reset loop counter
            __delay_ms(100);            
        }
    }

    return max_ppm;
}

/************************************
/ LED_Light_Meter
/ Function used to create a scale for the LED array
/ max_light is the ambient light, min_light is the LDR measurement when covered entirely
/ light_value is the current LDR reading
************************************/
unsigned int LED_Light_Meter(unsigned int max_light, unsigned int min_light, unsigned int light_value) {
    unsigned int LED_value = 0;
    unsigned int resolution;
    unsigned int step;

    /// Calculate resolution using floating-point division and then convert to integer
    resolution = ((max_light - min_light) / 9);

    // All LEDs on if light_value is above or equal to the maximum value
    if (light_value >= max_light) {
        LED_value = 511; // All LEDs on if light_value is above or equal to the maximum value
    } 
    else {
        for (step = 0; step < 9; step++) {
            if (light_value > min_light + resolution * step) {
                LED_value |= (1 << step); // Turn on the corresponding LED
            }
        }
    }
    return LED_value;
}

/************************************
/ highestBit
/ Function used find the most significant bit (MSB) in a binary number
/ value is the integer from which the MSB is extracted
************************************/
unsigned int highestBit(unsigned int value) {
    unsigned int msb = 0;
    while (value >>= 1) {
        msb++;
    }
    return (msb ? 1 << msb : 0); 
}