#include <xc.h>
#include "LCD.h"
#include <stdio.h>

/************************************
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

/************************************
 * Function to initialise the LCD after power on
************************************/
void LCD_Init(void)
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
 * Function to send string to LCD screen
************************************/
void LCD_scroll(char direction) {
    if (direction == 'R') {
        // Scroll Right (Shift display to the left)
        LCD_sendbyte(0b00011000, 0);
    } else if (direction == 'L') {
        // Scroll Left (Shift display to the right)
        LCD_sendbyte(0b00011100, 0);
    }
}

/************************************
 * Function takes a ADC value and works out the voltage to 2 dp
 * the result is stored in buf as ascii text ready for display on LCD
 * Note result is stored in a buffer using pointers, it is not sent to the LCD
************************************/
void ADC2String(char *buf, unsigned int ADC_val){
	//code to calculate the integer and fractions part of a ADC value
	// and format as a string using sprintf (see GitHub readme)
    
    unsigned int int_part=ADC_val/77; // i.e. 255 / 3.3V = 77 

    unsigned int frac_part=(ADC_val*100)/77 - int_part*100; //calculates fractional part
    
    sprintf(buf,"Voltage = %d.%02d",int_part,frac_part); //stores both separate parts in buf
    
    LCD_sendstring(buf); // Display voltage on the screen

}

void LCD_customChar(void){
    // Define custom character
    unsigned char headTop[8] = {
        0b00000,
        0b00000,
        0b00000,
        0b11100,
        0b00010,
        0b11001,
        0b11001,
        0b00001
    };

    unsigned char headBottom[8] = {
        0b00001,
        0b11001,
        0b11001,
        0b00010,
        0b11100,
        0b00000,
        0b00000,
        0b00000
    };
    
    unsigned char bodyTop[8] = {
        0b00000,
        0b00000,
        0b00000,
        0b11111,
        0b00000,
        0b00000,
        0b00000,
        0b00000
    };

    unsigned char bodyBottom[8] = {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b11111,
        0b00000,
        0b00000,
        0b00000
    };
    
    unsigned char tailTop[8] = {
        0b00000,
        0b00000,
        0b00000,
        0b00111,
        0b01000,
        0b10110,
        0b10110,
        0b10000
    };
    
    unsigned char tailBottom[8] = {
        0b10000,
        0b10110,
        0b10110,
        0b01000,
        0b00111,
        0b00000,
        0b00000,
        0b00000
    };
    
    unsigned char bodyPattTop[8] = {
        0b00000,
        0b00000,
        0b00000,
        0b11111,
        0b00100,
        0b00100,
        0b00100,
        0b00100
    };
    
    unsigned char bodyPattBottom[8] = {
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b11111,
        0b00000,
        0b00000,
        0b00000
    };
    
    
    // Creating the Custom Characters using the character Creation function
    LCD_createChar(0, headTop);
    LCD_createChar(1, headBottom);
    LCD_createChar(2, bodyTop);
    LCD_createChar(3, bodyBottom);
    LCD_createChar(4, tailTop);
    LCD_createChar(5, tailBottom);
    LCD_createChar(6, bodyPattTop);
    LCD_createChar(7, bodyPattBottom);
}

void LCD_createChar(unsigned char location, unsigned char charmap[]) {
    location &= 0x7; // we only have 8 locations 0-7
    LCD_sendbyte(0x40 | (location << 3), 0); // Set CGRAM address
    for (int i = 0; i < 8; i++) {
        LCD_sendbyte(charmap[i], 1); // Write character pattern
    }
}

void LCD_setCursor(int column, int line) {
    int baseAddress = 0x00; // Address of the first character of the first line
    if (line > 1) {
        baseAddress = 0x40; // Address of the first character of the second line
    }
    int address = baseAddress + column;
    LCD_sendbyte(0x80 | address, 0); // 0x80 is the command to set DDRAM address
}

void LCD_clear(void) {
    
    LCD_sendbyte(0x00,1);
    
}