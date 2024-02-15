#include <xc.h>
#include <pic18f67k40.h>
#include "LCD.h"
#include <stdio.h>




/*************************************
 * Function to toggle LCD enable bit on then off
 * when this function is called the LCD screen reads the data lines
************************************/
void LCD_E_TOG(void)
{
	//turn the LCD enable bit on
    LATCbits.LATC2=1; 
	__delay_us(2); //wait a short delay
	//turn the LCD enable bit off again
    LATCbits.LATC2=0;
}



/************************************
 * Function to initialise the LCD after power on
************************************/
void LCD_init(void)
{
    
    //Define LCD Pins as Outputs   
    TRISCbits.TRISC6=0;
    TRISCbits.TRISC2=0;
    TRISBbits.TRISB3=0;
    TRISBbits.TRISB2=0;
    TRISEbits.TRISE3=0;
    TRISEbits.TRISE1=0;
    
    //set all pins low (might be random values on start up, fixes lots of issues)
    LATCbits.LATC6=0;
    LATCbits.LATC2=0;
    LATBbits.LATB4 = 0;
    LATBbits.LATB5 = 0;
    LATBbits.LATB6 = 0;
    LATBbits.LATB7 = 0;
    
    //Initialisation sequence code
	// follow the sequence in the GitHub Readme picture for 4-bit interface.
	// first Function set should be sent with LCD_sendnibble (the LCD is in 8 bit mode at start up)
    LCD_sendnibble(0b0011);
    __delay_us(39); 
	// after this use LCD_sendbyte to operate in 4 bit mode
    LCD_sendbyte(0b00101000,0);
    __delay_us(39);
    LCD_sendbyte(0b00101000,0);
    __delay_us(37);
    LCD_sendbyte(0b00001100,0);
    __delay_us(37); 
    LCD_sendbyte(0b00000001,0);
    __delay_us(1530);
    LCD_sendbyte(0b00000110,0);

                                                        
	//remember to turn the LCD display back on at the end of the initialisation (not in the data sheet)
    LCD_sendbyte(0b1111,0);                 
}




/************************************
 * Function to set the 4-bit data line levels for the LCD
************************************/
void LCD_sendnibble(unsigned char number)
{

	//set the data lines here (think back to LED array output)
    // Now set the data lines according to the nibble's bits
    if(number & 0b0001) {LATBbits.LATB3 = 1;} else {LATBbits.LATB3 = 0;} // Check if the first bit is 1 and set to 0 if not

    if(number & 0b0010) {LATBbits.LATB2 = 1;} else {LATBbits.LATB2 = 0;} // Check if the second bit is 1 and set to 0 if not
        
    if(number & 0b0100) {LATEbits.LATE3 = 1;} else {LATEbits.LATE3 = 0;} // Check if the third bit is 1 and set to 0 if not
        
    if(number & 0b1000) {LATEbits.LATE1 = 1;} else {LATEbits.LATE1 = 0;} // Check if the fourth bit is 1 and set to 0 if not
        
    LCD_E_TOG();			//toggle the enable bit to instruct the LCD to read the data lines
    __delay_us(5);          //Delay 5uS
}





/************************************
 * Function to set the cursor to beginning of line 1 or 2
************************************/
void LCD_setline (char line)
{
    //Send 0x80 to set line to 1 (0x00 ddram address)
    if (line == 1) {LCD_sendbyte(0x80,0);}
    
    //Send 0xC0 to set line to 2 (0x40 ddram address)
    if (line == 2) {LCD_sendbyte(0xC0,0);}
}




/************************************
 * Function to send string to LCD screen
************************************/
void LCD_sendstring(char *string)
{
	//code here to send a string to LCD using pointers and LCD_sendbyte function
    while(*string != 0){  // While the data pointed to isn?t a 0x00 do below (strings in C must end with a NULL byte) 
		LCD_sendbyte(*string++,1); 	//Send out the current byte pointed to and increment the pointer
	}
}


/************************************
 * Function to send time information to the LCD screen
************************************/
void time2String(char *buf, unsigned int h, unsigned int day, unsigned int year, unsigned int leap){
    
    //printing hour and day on the first line
    LCD_setline(1);
    sprintf(buf,"H:%02d D:%03d",h,day); //stores hour and day in buf    
    LCD_sendstring(buf); //prints buf
    
    //printing Year and whether its Leap or normal on the second line
    LCD_setline(2);
    
    //If Leap is true it prints the year with Leap next to it
    if (leap == 1){
        sprintf(buf,"Y:%d Leap",year);
        LCD_sendstring(buf);
    }
    
    //If leap is false it prints the year with normal next to it
    else {
        sprintf(buf,"Y:%d Normal",year);
        LCD_sendstring(buf);
    }
    
    //In built delay in the function to slow down the refresh rate on the screen
    __delay_ms(200); 
}




/************************************
 * Function to send full 8-bit commands/data over the 4-bit interface
 * high nibble (4 most significant bits) are sent first, then low nibble sent
************************************/
void LCD_sendbyte(unsigned char Byte, char type)
{
    // set RS pin whether it is a Command (0) or Data/Char (1) using type argument
    if (type==1) {LATCbits.LATC6=1;} else {LATCbits.LATC6=0;}   
    
    // Send high bits of Byte using LCD_sendnibble function
    unsigned char highNibble = (Byte >> 4) ; // Shift the Byte 4 bits to the right to get the high nibble
    LCD_sendnibble(highNibble);

    // Send low bits of Byte using LCD_sendnibble function
    unsigned char lowNibble = Byte & 0b1111; // Mask the Byte with to get the low nibble
    LCD_sendnibble(lowNibble);
	
    __delay_us(50);               //delay 50uS (minimum for command to execute)
}
