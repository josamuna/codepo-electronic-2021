#include <xc.h>
#include "init.h"
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



#define CREATE_CMD_LENGTH  13
char createCmd[CREATE_CMD_LENGTH] = "AT+IOTCREATE\n";
#define CREATE_RESPONSE_LENGTH 21
char createSuccessResponse[CREATE_RESPONSE_LENGTH] = "+IOTCREATE: SUCCESS\r\n";

tsError_t tsCreate(void) {
    char response[CREATE_RESPONSE_LENGTH];
    tsError_t error;
    
    error = tsSendInstruction(createCmd, CREATE_CMD_LENGTH, response, CREATE_RESPONSE_LENGTH);
    
    if (error == NO_ERROR) {
        error = tsCheckResponse(response, createSuccessResponse, CREATE_RESPONSE_LENGTH);
    }
    
    return (error);
}



#define DESTROY_CMD_LENGTH  14
char destroyCmd[DESTROY_CMD_LENGTH] = "AT+IOTDESTROY\n";
#define DESTROY_RESPONSE_LENGTH 22
char destroySuccessResponse[DESTROY_RESPONSE_LENGTH] = "+IOTDESTROY: SUCCESS\r\n";

tsError_t tsDestroy(void) {
    char response[DESTROY_RESPONSE_LENGTH];
    tsError_t error;
    
    error = tsSendInstruction(destroyCmd, DESTROY_CMD_LENGTH, response, DESTROY_RESPONSE_LENGTH);
    
    if (error == NO_ERROR) {
        error = tsCheckResponse(response, destroySuccessResponse, DESTROY_RESPONSE_LENGTH);
    }
    
    return (error);
}



#define CONNECT_CMD_LENGTH  19
char connectCmd[CONNECT_CMD_LENGTH] = "AT+IOTCONNECT=true\n";
#define CONNECT_RESPONSE_LENGTH 22
char connectSuccessResponse[CONNECT_RESPONSE_LENGTH] = "+IOTCONNECT: SUCCESS\r\n";

tsError_t tsConnect(void) {
    char response[CONNECT_RESPONSE_LENGTH];
    tsError_t error;
    
    error = tsSendInstruction(connectCmd, CONNECT_CMD_LENGTH, response, CONNECT_RESPONSE_LENGTH);
    
    if (error == NO_ERROR) {
        error = tsCheckResponse(response, connectSuccessResponse, CONNECT_RESPONSE_LENGTH);
    }
    
    return (error);
}



#define DISCONNECT_CMD_LENGTH  17
char disconnectCmd[DISCONNECT_CMD_LENGTH] = "AT+IOTDISCONNECT\n";
#define DISCONNECT_RESPONSE_LENGTH 25
char disconnectSuccessResponse[DISCONNECT_RESPONSE_LENGTH] = "+IOTDISCONNECT: SUCCESS\r\n";

tsError_t tsDisconnect(void) {
    char response[DISCONNECT_RESPONSE_LENGTH];
    tsError_t error;
    
    error = tsSendInstruction(disconnectCmd, DISCONNECT_CMD_LENGTH, response, DISCONNECT_RESPONSE_LENGTH);
    
    if (error == NO_ERROR) {
        error = tsCheckResponse(response, disconnectSuccessResponse, DISCONNECT_RESPONSE_LENGTH);
    }
    
    return (error);
}



#define SUBSCRIBE_CMD_LENGTH 29
#define SUBSCRIBE_RESPONSE_LENGTH 24
char subscribeSuccessResponse[SUBSCRIBE_RESPONSE_LENGTH] = "+IOTSUBSCRIBE: SUCCESS\r\n";

tsError_t tsSubscribe(char *topic) {
    char response[SUBSCRIBE_RESPONSE_LENGTH];
    tsError_t error;
    char subscribeCmd[SUBSCRIBE_CMD_LENGTH];
    
    sprintf(subscribeCmd, "AT+IOTSUBSCRIBE=\"%s\",2\n", topic) ;
    error = tsSendInstruction(subscribeCmd, SUBSCRIBE_CMD_LENGTH, response, SUBSCRIBE_RESPONSE_LENGTH);
    
    if (error == NO_ERROR) {
        error = tsCheckResponse(response, subscribeSuccessResponse, SUBSCRIBE_RESPONSE_LENGTH);
    }
    
    return (error);
}



#define UNSUBSCRIBE_CMD_LENGTH 24
char unsubscribeCmd[UNSUBSCRIBE_CMD_LENGTH] = "AT+IOTUNSUBSCRIBE=\"WtD\"\n";
#define UNSUBSCRIBE_RESPONSE_LENGTH 26
char unsubscribeSuccessResponse[UNSUBSCRIBE_RESPONSE_LENGTH] = "+IOTUNSUBSCRIBE: SUCCESS\r\n";

tsError_t tsUnsubscribe(void) {
    char response[UNSUBSCRIBE_RESPONSE_LENGTH];
    tsError_t error;
    
    error = tsSendInstruction(unsubscribeCmd, UNSUBSCRIBE_CMD_LENGTH, response, UNSUBSCRIBE_RESPONSE_LENGTH);
    
    if (error == NO_ERROR) {
        error = tsCheckResponse(response, unsubscribeSuccessResponse, UNSUBSCRIBE_RESPONSE_LENGTH);
    }
    
    return (error);
}



#define GNSSpowerON_CMD_LENGTH 16
char GNSSpowerOnCmd[GNSSpowerON_CMD_LENGTH] = "AT+IOTCGNSPWR=1\n";
#define GNSSpowerON_RESPONSE_LENGTH 22
char GNSSpowerOnSuccessResponse[GNSSpowerON_RESPONSE_LENGTH] = "+IOTCGNSPWR: SUCCESS\r\n";

tsError_t tsGNSSpowerOn(void) {
    char response[GNSSpowerON_RESPONSE_LENGTH];
    tsError_t error;
    
    error = tsSendInstruction(GNSSpowerOnCmd, GNSSpowerON_CMD_LENGTH, response, GNSSpowerON_RESPONSE_LENGTH);
    
    if (error == NO_ERROR) {
        error = tsCheckResponse(response, GNSSpowerOnSuccessResponse, GNSSpowerON_RESPONSE_LENGTH);
    }
    
    return (error);
}

#define GNSS_CMD_LENGTH 14
char GNSSCmd[GNSS_CMD_LENGTH] = "AT+IOTCGNSINF\n";
#define GNSS_CMD_RESPONSE_LENGTH 18
char GNSSCmdSuccessResponse[GNSS_CMD_RESPONSE_LENGTH] = "+IOTCGNSINF: 1,1";

tsError_t tsGNSSCmd(void) {
    char response[GNSS_CMD_RESPONSE_LENGTH];
    tsError_t error;
    
    error = tsSendInstruction(GNSSCmd, GNSS_CMD_LENGTH, response, GNSS_CMD_RESPONSE_LENGTH);
    
    if (error == NO_ERROR) {
        error = tsCheckResponse(response, GNSSCmdSuccessResponse, GNSS_CMD_RESPONSE_LENGTH);
    }
    
    return (error);
}



#define GNSSpowerOFF_CMD_LENGTH 16
char GNSSpowerOffCmd[GNSSpowerOFF_CMD_LENGTH] = "AT+IOTCGNSPWR=0\n";
#define GNSSpowerOFF_RESPONSE_LENGTH 22
char GNSSpowerOffSuccessResponse[GNSSpowerOFF_RESPONSE_LENGTH] = "+IOTCGNSPWR: SUCCESS\r\n";

tsError_t tsGNSSpowerOff(void) {
    char response[GNSSpowerOFF_RESPONSE_LENGTH];
    tsError_t error;
    
    error = tsSendInstruction(GNSSpowerOffCmd, GNSSpowerOFF_CMD_LENGTH, response, GNSSpowerOFF_RESPONSE_LENGTH);
    
    if (error == NO_ERROR) {
        error = tsCheckResponse(response, GNSSpowerOffSuccessResponse, GNSSpowerOFF_RESPONSE_LENGTH);
    }
    
    return (error);
}



#define PUBLISH_RESPONSE_LENGTH 22
char publishSuccessResponse[PUBLISH_RESPONSE_LENGTH] = "+IOTPUBLISH: SUCCESS\r\n";

tsError_t tsPublish(char *topic, char *caseId, double soc, double autonomy, char *latitude, char *longitude, int16_t mode, int16_t intervalBatSec, int16_t intervalSendingHour, int16_t boolAlertPercentage) {
    char response[UNSUBSCRIBE_RESPONSE_LENGTH];
    tsError_t error;
    int16_t length;
    char publishCmd[250];

    if (mode == 1) {
        length = sprintf(publishCmd, "AT+IOTPUBLISH=\"%s\",1,\"{&caseid&: %s, &SOC&: %.2f, &autonomy&: %.2f, &latitude&: hidden, &longitude&: hidden, &mode&: %d, &interval_bat_s&: %d, &interval_sending_h&: %d, &alert_battery&: %d}\",true\n",
                                 topic, caseId, soc, autonomy, mode, intervalBatSec, intervalSendingHour, boolAlertPercentage);
    } else if (mode == 2) {
        length = sprintf(publishCmd, "AT+IOTPUBLISH=\"%s\",1,\"{&caseid&: %s, &SOC&: %.2f, &autonomy&: %.2f, &latitude&: %s, &longitude&: %s, &mode&: %d, &interval_bat_s&: %d, &interval_sending_h&: %d, &alert_battery&: %d}\",true\n",
                                 topic, caseId, soc, autonomy, latitude, longitude, mode, intervalBatSec, intervalSendingHour, boolAlertPercentage);
    }
    error = tsSendInstruction(publishCmd, length, response, PUBLISH_RESPONSE_LENGTH);

    if (error == NO_ERROR) {
        error = tsCheckResponse(response, publishSuccessResponse, PUBLISH_RESPONSE_LENGTH);
    }
    
    return (error);
}



#define INFO_CMD_LENGTH  11
char infoCmd[INFO_CMD_LENGTH] = "AT+IOTINFO\n";

tsError_t tsInfo(char* response, int16_t length) {
    tsError_t error;
    
    error = tsSendInstruction(infoCmd, INFO_CMD_LENGTH, response, length);
    
    return (error);
}



// LOW_LEVEL FUNCTIONS IMPLEMENTATION
/////////////////////////////////////

//* Flush the input buffer of the UART
void tsFlushRx(void) {
    while ( uart1RxDataAvailable() ) {
        uart1GetChar();
    }
}

/* Used to send AT instructions to the Thingstream Click through the UART channel */
tsError_t tsSendInstruction(char cmd[], int16_t cmd_length, char* response, int16_t responseLength) {
    int16_t i;
    tsError_t error = NO_ERROR;
    
    // Flush the Rx buffer to be sure it's empty before sending a new command
    tsFlushRx();
    
    // Sending the AT instruction
    for(i=0; i<cmd_length; i++) {
        while(!uart1TxReady());
        uart1SendChar(cmd[i]);
    }
    
    error = tsReceiveMsg(response, responseLength);
    
    return(error);
}


/* Used to receive the asynchronous message from the Thingstream Click (message from the server to this device */
tsError_t tsReceiveMsg(char* response, int16_t responseLength) {
    int16_t i;
    char c;
    tsError_t error = NO_ERROR;
    clock_t t0, t1;
    
    c = 0;
    i = 0;
    t0 = clock();
    // The thingstream module response should end with 0x0A character
    while ( (c != 0x0A) && (error == NO_ERROR) ) { 
        // Wait for the next character of the response
        while( !uart1RxDataAvailable() && (error == NO_ERROR) ) {
            t1 = clock();
            if ( difftime(t1, t0) > TS_TIMEOUT ) {
                error = TIMEOUT_ERROR;
            }
        }
        if ( uart1RxDataAvailable() ) {
            c = uart1GetChar();
            if (i < responseLength){
                response[i++] = c;
            } else {
                error = RESPONSE_LENGTH_ERROR;
            }
        }
    }
    
    return(error);
}


tsError_t tsCheckResponse(char* response, char* expectedResponse, int16_t length) {
    tsError_t error = NO_ERROR;
    int16_t i = 0;
    
    while ( (i < length) && (error == NO_ERROR) ) {
        if (response[i] != expectedResponse[i]) {
            error = RESPONSE_ERROR;
        }
        i++;
    }
    
    return (error);
}