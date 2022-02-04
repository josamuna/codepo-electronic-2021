/* 
 * File:   init.h
 * Author: nicolaswallemacq
 * Inspired by: mosee
 */

#ifndef INIT_H
#define	INIT_H

    #define U1TX_PPS_ID     (3)
    #define U2TX_PPS_ID     (5)

    //#define SENSORS_ON               LATBbits.LATB7 //CANDIDATE FOR SUPPRESSION
    //#define SENSORS_ON_TRIS          TRISBbits.TRISB7   //CANDIDATE FOR SUPPRESSION

    #define THINGSTREAM_ON          LATBbits.LATB10
    #define THINGSTREAM_ON_TRIS     TRISBbits.TRISB10

    
    //* CPU cycle frequency. Mandatory to use __delay_ms() or __delay_us())
    #define FCY (3685000ul)


    /**
     * Configures the I/O pins for the connected peripherals and to minimse
     * the power comsumption during sleep mode
     */
    void pinInit(void);
    
    //#define SENSORS_POWER_ON()        SENSORS_ON_TRIS=0; //CANDIDATE FOR SUPPRESSION
    //#define SENSORS_POWER_DOWN()       SENSORS_ON_TRIS=1; //CANDIDATE FOR SUPPRESSION
    #define THINGSTREAM_POWER_ON()    THINGSTREAM_ON_TRIS=0;
    #define THINGSTREAM_POWER_DOWN()   THINGSTREAM_ON_TRIS=1;
    



#endif	/* INIT_H */

