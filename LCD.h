#ifndef _LCD_H
#define _LCD_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void LCD_E_TOG(void);
void LCD_sendnibble(unsigned char number);
void LCD_sendbyte(unsigned char Byte, char type);
void LCD_Init(void);
void LCD_setline (char line);	
void LCD_sendstring(char *string);
void LCD_scroll(char direction);
void LCD_clear(void);
void ADC2String(char *buf, unsigned int number);
void LCD_customChar(void);
void LCD_createChar(unsigned char location, unsigned char charmap[]);
void LCD_setCursor(int column, int line);


#endif
