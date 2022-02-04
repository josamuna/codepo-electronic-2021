#include <xc.h>
#include "thingstream.h"
#include "uart1.h"

#include <libq.h>
#include <dsp.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h> 
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

float clock_factor_2 = 3685001.4;

/* Used to send AT instructions to the Thingstream Click through the UART channel */
int16_t tsSendInstruction(char cmd[], int cmd_length, char* echo, int echo_length) {
    int16_t i;
    unsigned char c;
    // Sending the AT instruction
    for(i=0; i<cmd_length; i++) {
        while(!uart1TxReady());
        uart1SendChar(cmd[i]);
    }
    c = 0;
    i=0;

    while ( c != 0x0A) { 
        while( !uart1RxDataAvailable()); // || t2-t1 < time_limit (Alban's idea)
        c = uart1GetChar();
        i++;
        if (i<=echo_length){
            echo[i-1]=c; 
        }
        
    }
    echo[i]='\0'; 
    return(1);
}

/* Used to receive the asynchronous message from the Thingstream Click (message from the server to this device */
int16_t tsReceiveMsg(char* response) {
    int16_t i;
    char c;
    c = 0;
    i = 0;
    while ( c != 0x0A ) {
        while( !uart1RxDataAvailable() );
        c = uart1GetChar();
        i++;
        response[i-1] = c;   
    }
    response[i] = '\0'; 
    return(1);
}