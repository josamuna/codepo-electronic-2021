/* 
 * File: thingstream.h  
 * Author: Cécile Castiaux
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef THINGSTREAM_H
#define	THINGSTREAM_H

#include <xc.h> // include processor files - each processor file is guarded.  


/* Defining enable & disable functions for the UART */
#define THINGSTREAM_UART_ENABLE()        U1MODEbits.UARTEN=1;U1STAbits.UTXEN=1;
#define THINGSTREAM_UART_DISABLE()       U1STAbits.UTXEN=0;U1MODEbits.UARTEN=0;

typedef enum {
    NO_ERROR = 0,
    TIMEOUT_ERROR = 1,
    RESPONSE_LENGTH_ERROR = 2,
    RESPONSE_ERROR = 3
} tsError_t;

#define LATITUDE_LENGTH 10
#define LONGITUDE_LENGTH 10
typedef struct {
    char latitude[LATITUDE_LENGTH];
    char longitude[LONGITUDE_LENGTH];
} gpsCoord_t;

typedef enum {
    BAT_GPS = 1,
    BAT = 2,
    ECONOMY = 3,
    CALIBRATION = 4
} workMode_t;

tsError_t tsCreate(void);
tsError_t tsDestroy(void);
tsError_t tsConnect(void);
tsError_t tsDisconnect(void);
tsError_t tsSubscribe(char *topic);
tsError_t tsUnsubscribe(void);
tsError_t tsGNSSpowerOn(void);
tsError_t tsGNSSpowerOff(void);
tsError_t tsGNSSCmd(gpsCoord_t* gpsCoord);
tsError_t tsPublish(char *topic, char *caseId, double energyIn, double energyOut, gpsCoord_t coord, workMode_t mode, int16_t intervalBatSec, int16_t intervalSendingHour, double temperature);
tsError_t tsInfo(char* response, int16_t length);

// LOW_LEVEL FUNCTIONS PROTOTYPES
/////////////////////////////////////////
void tsFlushRx(void);
tsError_t tsSendInstruction(char cmd[], int16_t cmd_length, char* response, int16_t responseLength);
tsError_t tsReceiveMsg(char* response, int16_t responseLength);
tsError_t tsCheckResponse(char* response, char* expectedResponse, int16_t length);
tsError_t gpsGetCoord(gpsCoord_t* gpsCoord, char* response);

/*
void write_string(char *dest, char text[]);
int  getSubString(char *source, char *target,int from, int to);
void extract_relevant_data(char* relevant_data, int *mode, int *interval_bat_s, int *interval_sending_h);
void extract_mode_and_intervals(char message_received[], int *mode, int *interval_bat_s, int *interval_sending_h);
void extract_lat_long(char *latitude, char *longitude, char echo_gnss[]);
 */
/*
 non utilisé dans le code des étudiants
#define ASKCONNECT_CMD_LENGTH  15
char askconnectCmd[ASKCONNECT_CMD_LENGTH] = "AT+IOTCONNECT?\n";
#define ASKCONNECT_ECHO_LENGTH 17
char echo_askconnect_success[ASKCONNECT_ECHO_LENGTH] = "+IOTCONNECT: TRUE";

#define RECEIVE_ECHO_LENGTH 11
char echo_receive_success[RECEIVE_ECHO_LENGTH] = "+IOTRECEIVE";
*/
#endif	/* THINGSTREAM_H */








