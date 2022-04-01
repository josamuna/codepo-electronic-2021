
// DSPIC33FJ128MC802 Configuration Bit Settings

// 'C' source line config statements

// FBS
#pragma config BWRP = WRPROTECT_OFF     // Boot Segment Write Protect (Boot Segment may be written)
#pragma config BSS = NO_FLASH           // Boot Segment Program Flash Code Protection (No Boot program Flash segment)
#pragma config RBS = NO_RAM             // Boot Segment RAM Protection (No Boot RAM)

// FSS
#pragma config SWRP = WRPROTECT_OFF     // Secure Segment Program Write Protect (Secure segment may be written)
#pragma config SSS = NO_FLASH           // Secure Segment Program Flash Code Protection (No Secure Segment)
#pragma config RSS = NO_RAM             // Secure Segment Data RAM Protection (No Secure RAM)

// FGS
#pragma config GWRP = OFF               // General Code Segment Write Protect (User program memory is not write-protected)
#pragma config GSS = OFF                // General Segment Code Protection (User program memory is not code-protected)

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Mode (Internal Fast RC (FRC))
#pragma config IESO = OFF               // ###### Internal External Switch Over Mode (Start-up device with user-selected oscillator source)

// FOSC
#pragma config POSCMD = NONE            // Primary Oscillator Source (Primary Oscillator Disabled)
//#pragma config OSCIOFNC = ON            // ##### OSC2 Pin Function (OSC2 pin has digital I/O function)
#pragma config OSCIOFNC = OFF //MODIFFFFFFFFF
#pragma config IOL1WAY = ON             // Peripheral Pin Select Configuration (Allow Only One Re-configuration)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Both Clock Switching and Fail-Safe Clock Monitor are disabled)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler (1:32,768)
#pragma config WDTPRE = PR128           // WDT Prescaler (1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = ON              // ##### Watchdog Timer Enable (Watchdog timer always enabled)
// #pragma config FWDTEN = OFF 

// FPOR
#pragma config FPWRT = PWR128           // POR Timer Value (128ms)
#pragma config ALTI2C = OFF             // Alternate I2C  pins (I2C mapped to SDA1/SCL1 pins)
#pragma config LPOL = ON                // Motor Control PWM Low Side Polarity bit (PWM module low side output pins have active-high output polarity)
#pragma config HPOL = ON                // Motor Control PWM High Side Polarity bit (PWM module high side output pins have active-high output polarity)
#pragma config PWMPIN = ON              // Motor Control PWM Module Pin Mode bit (PWM module pins controlled by PORT register at device Reset)

// FICD
#pragma config ICS = PGD1               // Comm Channel Select (Communicate on PGC1/EMUC1 and PGD1/EMUD1)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG is Disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "init.h"

void pinInit(void) {    
    AD1PCFGL = 0xFFFF;  // all pins in digital
    _PCFG1 = 0; // to allow AN0 and AN1 to be read at the same time
    LED_TRIS = 0;

    // Thingstream click is connected to UART1
#ifdef OLD_PROTO
    // Configure power switches PIO
    THINGSTREAM_ON_TRIS = 1;
    THINGSTREAM_ON = 0;
    // configure the click-board UART pins
    _TRISB14 = 1;
    _TRISB15 = 1;
    _RP15R = U1TX_PPS_ID;
    _U1RXR = 14;
#else
    // Configure power switches PIO
    THINGSTREAM_ON_TRIS = 1;
    THINGSTREAM_ON = 1;
    // configure the click-board UART pins
    _TRISB7 = 1;
    _TRISB6 = 1;
    _RP7R = U1TX_PPS_ID;
    _U1RXR = 6;
#endif
}
