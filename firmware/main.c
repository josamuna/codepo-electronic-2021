/* 
 * File:   main.c
 * Author: nicolaswallemacq
 * check this link for fsm template https://www.microchip.com/forums/m520736.aspx
 * Created on May 2, 2021, 3:59 PM
 * This is the main function for the monitoring of the Zendure battery. 
 */

/* Importing libraries specific for the hardware & the microcontroller*/
#include "init.h"
#include "uart1.h"
#include "adc.h" 
#include "thingstream.h" 
#include <xc.h>
#include <libpic30.h>   // FCY definition is in init.h, contains __delay_ms())
#include <time.h>       // contains clock()
// Importing other libraries for message decoding
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// FWDT
#pragma config WDTPOST = PS1024        // Watchdog Timer Postscaler (1:32,768)
#pragma config WDTPRE = PR128           // WDT Prescaler (1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = ON              // ##### Watchdog Timer Enable (Watchdog timer always enabled)


/* Main function: implemented as a finite state machine*/
int main(void) {

    /* Serial & Pins Setup */
    pinInit();

    while (1) {
        LED_ON();
        __delay_ms(904); 
        LED_OFF();
        //__delay_ms(1000); 
        Sleep();
    }
    return (0);
}
