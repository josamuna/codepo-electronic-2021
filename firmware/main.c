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
#include <p33FJ128MC802.h>


/* Le µC est mis en IDLE pendant 4 secondes pour économiser l'énergie
 * La LED est allumée pendant 0.5s pour garder le powerPack actif
 * C'est la durée minimum expérimentale (250ms ne marche pas)
 * La durée max pour le powerpack est 13s
 * Le mode IDLE est nécessaire pour que le timer1 reste actif
 * 
 * on utilise FRC/16 comme horloge pour limiter la consommation et atteindre
 * des durées plus longue ave le timer1
 */

void __attribute__((interrupt, auto_psv)) _T1Interrupt( void ) {
    _T1IF = 0;
    LED_ON();
}


/* Main function: implemented as a finite state machine*/
int main(void) {
    T1CONbits.TCKPS = 2;    // PS = 1:256
    PR1 = 35985;//14394;            // PR = FCY*T/PS - 1 = 230312Hz*4s/256 - 1
    _T1IF = 0;
    _T1IE = 1;
    T1CONbits.TON = 1;
    
    /* Serial & Pins Setup */
    pinInit();

    while (1) {
        Idle();
        __delay_ms(500);
        LED_OFF();
    }
    return (0);
}
