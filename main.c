#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include "serial.h"
#include "LCD.h"
#include <stdio.h>
#include "comparator.h"
#include "LEDarray.h"
#include "ADC.h"


#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  


void main(void) {
    //initializing all necessary functions
    initUSART4();
    LCD_Init();
    ADC_init();
    
    //defining variables
    char buf[50];
    unsigned int ADC_val;
    
    
    while (1){
        //set variable equal to ADC value
        ADC_val = ADC_getval();
        
        //writing on first line
        LCD_setline(1);
        
        //convert ADC value to string
        ADC2String(buf, ADC_val);
        
        
        //send the string over USART
        sendStringSerial4(buf); //Send ADC VAL to realterm program
        
        __delay_ms(1000); // Wait for 1 second

    }
}

