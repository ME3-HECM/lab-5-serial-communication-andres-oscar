#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include "serial.h"
#include "LCD.h"
#include <stdio.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void) {
    //initializing all necessary functions
    initUSART4();
    LCD_init();
    
    //defining variables
    char *data;
    
    //writing on first line
    LCD_setline(1);
    
    while (1){
        //receiving and printing charcaters
        char receivedChar = getCharSerial4();
        LCD_sendbyte(receivedChar,1);
    }
}

