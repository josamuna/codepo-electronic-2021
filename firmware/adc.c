#include "xc.h"
#include "adc.h"

void adcInit(int mode) {
    if ( (mode == ADC_MANUAL_SAMPLING) || (mode == ADC_TIMER3_SAMPLING) ) {
        AD1CON1bits.AD12B = 0;          // Convertisseur sur 10 bits
        AD1CON3bits.ADCS = 3;           // Diviseur d'horloge de l'adc
        AD1CON1bits.ASAM = 1;           // auto sample activÈ
        AD1CSSLbits.CSS0 = 1;           // Conversion de la patte AN0 uniquement
        AD1PCFGLbits.PCFG0 = 0;         // AN0 en mode analogique
        AD1CON1bits.SSRC = mode;        // choix du mode
        AD1CON1bits.ADON = 1;           // Activation de l'ADC
        while(AD1CON1bits.SAMP == 0);   // On attend que la configuration soit finie
    }
}


void adcStart(int pin) {
    //choose pin you want to read by putting the right int (AN0--> 0, AN1 --> 1,...)
    AD1CHS0bits.CH0SA = pin; 
    AD1CON1bits.SAMP = 0;
}


int adcConversionDone(void) {
    return (AD1CON1bits.DONE);
}


int adcRead(void) {
    if (AD1CON1bits.DONE) {
        AD1CON1bits.DONE = 0;
        return ADC1BUF0;
    } else {
        return (-32768);
    }
}