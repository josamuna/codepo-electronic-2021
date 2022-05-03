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
    #define FCY (230312ul)

    //* Timeout to stop waiting Thingstream-click response to an AT command, in ticks
    #define TS_TIMEOUT  (100*FCY)     // 1 second


    #define U1TX_PPS_ID     (3)

    #define THINGSTREAM_ON          LATBbits.LATB10
    #define THINGSTREAM_ON_TRIS     TRISBbits.TRISB10

    #define LED_ON()    _LATB5 = 1;
    #define LED_OFF()   _LATB5 = 0;
    #define LED_TRIS    _TRISB5

    #define ILOAD_GAIN      (51*0.033*3.3/1023)     // La résistance de mesure vaut 33mOhm et l'amplificatuer différentiel a un gain de 51
    #define ICHARGE_GAIN    (15*0.033*3.3/1023)     // La résistance de mesure vaut 33mOhm et l'amplificatuer différentiel a un gain de 15
    #define VCHARGE_GAIN    (1*3.3/1023)
    #define VLOAD_GAIN      (1*3.3/1023)
    #define ICHARGE_CHANNEL 0           // la mesure de Icharge se fait sur AN0
    #define ILOAD_CHANNEL   1           // la mesure de Iload se fait sur AN1
    #define VCHARGE_CHANNEL 2           // la mesure de Vcharge se fait sur AN2
    #define VLOAD_CHANNEL   3           // la mesure de Vload se fait sur AN3

    /**
     * Configures the I/O pins for the connected peripherals and to minimse
     * the power comsumption during sleep mode
     */
    void pinInit(void);
    
    #define THINGSTREAM_POWER_ON()    THINGSTREAM_ON_TRIS=0;
    #define THINGSTREAM_POWER_DOWN()   THINGSTREAM_ON_TRIS=1;


#endif	/* INIT_H */

