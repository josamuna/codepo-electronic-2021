/* 
 * File:   init.h
 * Author: nicolaswallemacq
 * Inspired by: mosee
 */

#ifndef INIT_H
#define	INIT_H
#include <xc.h>

    //#define OLD_PROTO

    //* CPU cycle frequency. Mandatory to use __delay_ms() or __delay_us())
    //#define FCY (3685000ul)
    #define FCY (230312ul)
    //* Timeout to stop waiting Thingstream-click response to an AT command, in ticks
    #define TS_TIMEOUT  (100*FCY)     // 1 second


    #define U1TX_PPS_ID     (3)

    #define THINGSTREAM_ON          LATBbits.LATB10
    #define THINGSTREAM_ON_TRIS     TRISBbits.TRISB10

    #define LED_ON()    _LATB5 = 1;
    #define LED_OFF()   _LATB5 = 0;
    #define LED_TRIS    _TRISB5
    #define LED         _LATB5

    /**
     * Configures the I/O pins for the connected peripherals and to minimse
     * the power comsumption during sleep mode
     */
    void pinInit(void);
    
    #define THINGSTREAM_POWER_ON()    THINGSTREAM_ON_TRIS=0;
    #define THINGSTREAM_POWER_DOWN()   THINGSTREAM_ON_TRIS=1;


#endif	/* INIT_H */

