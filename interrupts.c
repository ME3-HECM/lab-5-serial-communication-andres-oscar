#include <xc.h>
#include "interrupts.h"
#include "serial.h"

/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
void Interrupts_init(void)
{
	// turn on global interrupts, peripheral interrupts and the interrupt source 
	// It's a good idea to turn on global interrupts last, once all other interrupt configuration is done.
    PIE2bits.C1IE = 1; // comparator 1
    IPR2bits.C1IP = 1; // high priority
    INTCONbits.PEIE = 1; // peripheral interrupts
    INTCONbits.GIE = 1; // global interrupts
}

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/
void __interrupt(high_priority) HighISR()
{
	//add your ISR code here i.e. check the flag, do something (i.e. toggle an LED), clear the flag...
    if (PIR2bits.C1IF == 1) {
        LATHbits.LATH3 = !LATHbits.LATH3;//change value for the H3 LED 
        PIR2bits.C1IF = 0;
    }
    // When reg is cleared this flag is set and the ISR 
    if (PIR4bits.TX4IF) {
        //This sends out the current pointed char in the buffer
        TX4REG=getCharFromTxBuf();
    } 
    if (dataFlag&0){
        //when buffer is empty turn off the interrupt flag
        PIE4bits.TX4IE=0;
    } 

}