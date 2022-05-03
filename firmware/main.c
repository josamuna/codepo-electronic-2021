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



#define COMM_PERIOD_H       4                       // Période d'envoi des messages, en heures
#define TIMER1_PERIOD_S     10                      // Durée d'un "sleep", en secondes
#define GPS_MAX_TRY         (120/TIMER1_PERIOD_S)   // max duration fot trying to get GPS coordinates
uint16_t commSleepThreshold = (COMM_PERIOD_H * 3600ul) / TIMER1_PERIOD_S;    // number of timer1 period between messages

// MQTT topics
char publishTopic[] = "DtW\0";
char caseId[9] = "FICT8C79\0";
/* Send the µC in sleep mode, then after wake-up, update the measurements*/
void sleepFunction(void);

void write_string(char *dest, char text[]);
int getSubString(char *source, char *target, int from, int to);
void extract_relevant_data(char* relevant_data);
void extract_data_msg(char message_received[]);
void extract_lat_long(char *lat, char *longi, char echo_gnss[]);

/* Defining the different states of the finite state machine */
typedef enum {
    IDLE,
    MEASURE,
    ESTABLISHCONNECTION,
    LISTENING,
    GPS_INIT,
    GPS_READ,
    SENDING,
    DESTROY_CONNECTION,
    ERROR
} fsmState_t;


float isrChargeEnergy = 0;
float isrLoadEnergy = 0;
uint16_t isrCommSleepCount = 0;
void __attribute__((interrupt, auto_psv)) _T1Interrupt( void ) {
    float current;
    float voltage;
    
    LED_ON();
    // Mesure du courant
    AD1CHS0bits.CH0SA = ICHARGE_CHANNEL; 
    AD1CON1bits.SAMP = 0;
    while (!AD1CON1bits.DONE);
    AD1CON1bits.DONE = 0;
    current = ICHARGE_GAIN * ADC1BUF0;
    // Mesure de la tension
    AD1CHS0bits.CH0SA = VCHARGE_CHANNEL;
    while (!AD1CON1bits.DONE);
    AD1CON1bits.DONE = 0;
    voltage = VCHARGE_GAIN * ADC1BUF0;
    isrChargeEnergy += voltage * current * TIMER1_PERIOD_S;

    // Mesure du courant
    AD1CHS0bits.CH0SA = ILOAD_CHANNEL; 
    AD1CON1bits.SAMP = 0;
    while (!AD1CON1bits.DONE);
    AD1CON1bits.DONE = 0;
    current = ILOAD_GAIN * ADC1BUF0;
    // Mesure de la tension
    AD1CHS0bits.CH0SA = VLOAD_CHANNEL;
    while (!AD1CON1bits.DONE);
    AD1CON1bits.DONE = 0;
    voltage = VLOAD_GAIN * ADC1BUF0;
    isrLoadEnergy += voltage * current * TIMER1_PERIOD_S;
    
    isrCommSleepCount++;
}


/* Main function: implemented as a finite state machine*/
int main(void) {
    fsmState_t fsmState;
    tsError_t tsError;
    float chargeEnergy, loadEnergy;     // Energy given by the charger/fed to the load since last message
    gpsCoord_t gpsCoord;    // GPS coordinates, given by the click-board
    char errStr[] = "201";      // Error code that will replace ths GPS coordinates if GPS initialisation of click-board fails
    workMode_t workMode = BAT;  // Current work mode
    float temperature = 20.22;  // temperature in the case
    uint16_t gpsCount;



    /* Serial & Pins Setup */
    pinInit();
    LED_ON();
    adcInit(ADC_MANUAL_SAMPLING); // Configuration de L'ADC pour un declenchement par adcStart())
    uart1Config(115200, FCY, UART_8N1_NO_HANDSHAKE);

    /* Starting the finite state machine by entering the AWAKE state */
    fsmState = IDLE;

    while (1) {
        switch (fsmState) {
            /** Idle state: 
             * Go to Idle mode to minimize power consumption.
             * Timer1 interrupt gets us out of IDLE (and turns the LED on to
             * refresh the power pack.
             * After 500ms, turn the LED off, then
             * check if it's time to send message */
            case IDLE:
                Idle(); 
                __delay_ms(500);
                LED_OFF();
                if (isrCommSleepCount > commSleepThreshold) {
                    isrCommSleepCount = 0;  // no need for a critical section (atomic operation)
                    fsmState = ESTABLISHCONNECTION;
                }
                break;

            case ESTABLISHCONNECTION:
                THINGSTREAM_POWER_ON(); // SWITCHING ON THE COMMUNICATION MODULE
                __delay_ms(100);
                THINGSTREAM_UART_ENABLE(); // SWITCHING ON THE UART CHANNEL

                tsError = tsCreate();
                if (tsError == NO_ERROR) {
                    tsError = tsConnect();
                }
                if (tsError == NO_ERROR) {
                    fsmState = LISTENING;
                } else {
                    fsmState = ERROR;
                }
                break;

            case LISTENING:
                /* Listening for messages sent by the server to this device. A message can only be received 
                  once subscribed to the topic the message was posted on */
/*                tsError = tsSubscribe(caseId0);
                if (tsError == NO_ERROR) {
                    tsError = tsReceiveMsg(receivedMsg, MSG_RECEIVED_LENGTH);
                    if (tsError == NO_ERROR) {
                        //extract_data_msg(receivedMsg); // extraction of the relevant data
                    }
                }
                tsError = tsUnsubscribe();
*/
                if (tsError != NO_ERROR) {
                    fsmState = ERROR;
                }
                else if (workMode == BAT) {
                    fsmState = SENDING;
                } else if (workMode == BAT_GPS) {
                    fsmState = GPS_INIT;
                } else { //for mode ECONOMY
                    fsmState = DESTROY_CONNECTION;
                }
                break;

            case SENDING:
                _T1IE = 0;
                chargeEnergy = isrChargeEnergy;
                isrChargeEnergy = 0;
                loadEnergy = isrLoadEnergy;
                isrLoadEnergy = 0;
                _T1IE = 1;
                tsError = tsPublish(publishTopic, caseId, chargeEnergy, loadEnergy, gpsCoord, workMode, TIMER1_PERIOD_S, COMM_PERIOD_H, temperature);

                if (tsError == NO_ERROR) {
                    fsmState = DESTROY_CONNECTION;
                } else {
                    fsmState = ERROR;
                }
                break;


            case ERROR:
            case DESTROY_CONNECTION:
                /* DESTROYCONNECTION state: destruction of the connection we have with the Thingstream Server */
                //tsError = tsDisconnect();
                tsError = tsDestroy();
                THINGSTREAM_UART_DISABLE(); // SWITCHING OF THE UART CHANNEL
                THINGSTREAM_POWER_DOWN();   // SWITCHING OFF THE COMMUNICATION MODULE
                fsmState = IDLE;
                break;

            case GPS_INIT:
                tsError = tsGNSSpowerOn();
                if (tsError == NO_ERROR) {
                    gpsCount = 0;
                    fsmState = GPS_READ;
                } else {
                    strcpy(gpsCoord.longitude, errStr);
                    strcpy(gpsCoord.latitude, errStr);
                    fsmState = SENDING;
                }
                break;
                
            case GPS_READ:
                Idle();
                if (gpsCount++ < GPS_MAX_TRY) {
                    if (tsGNSSCmd(&gpsCoord) == NO_ERROR) {
                        // tsGNSSpowerOff();    // creates an "unespected error" from the click board
                        fsmState = SENDING;
                    }
                } else {
                    fsmState = SENDING;
                }
                break;
        }
    }
    return (0);
}


/* Used to write a string into a char array*/
void write_string(char *dest, char text[]) {
    int len = strlen(text);
    int i = 0;
    for (i = 0; i < len; i++) {
        dest[i] = text[i];
    }
    dest[len] = '\0';
}

/*
// Used to extract a substring located between the indexes 'from' and 'to' from 'source' and store it into 'target'. 
int getSubString(char *source, char *target, int from, int to) {
    int length = 0;
    int i = 0, j = 0;
    length = strlen(source);
    if (from < 0 || from > length) {
        printf("Invalid \'from\' index\n");
        return 1;
    }
    if (to > length) {
        printf("Invalid \'to\' index\n");
        return 1;
    }
    for (i = from; i <= to; i++) {
        target[j] = source[i];
        j++;
    }
    target[j] = '\0';
    return 0;
}
*/
/*
// Used to extract the relevant data (mode, interval_bat_s, interval_sending_h) from the message received from the web 
void extract_relevant_data(char* relevant_data) {
    //int table[3];
    int table[5];
    int mode_prev = workMode;
    //int interval_bat_s_prev = interval_bat_s;
    //int interval_sending_h_prev = interval_sending_h;
    int temp = 1;
    int a = 0;
    int len = strlen(relevant_data);
    int i = 0;
    
    for (i = 0; i < len; i++) {
        if (relevant_data[i] == ',') {
            table[a] = i;
            a++;
        }
    }

    temp = atoi(&relevant_data[0]);
    if (temp >= mode_lower_bound && temp <= mode_upper_bound) {
        workMode = temp;
    } else {
        workMode = mode_prev;
    }

    temp = atoi(&relevant_data[table[0] + 1]);
    if (temp >= interval_bat_s_lower_bound && temp <= interval_bat_s_upper_bound) {
//        interval_bat_s = temp;
    } else {
//        interval_bat_s = interval_bat_s_prev;
    }

    temp = atoi(&relevant_data[table[1] + 1]);
    if (temp >= interval_sending_h_lower_bound && temp <= interval_sending_h_upper_bound) {
//        interval_sending_h = temp;
    } else {
//        interval_sending_h = interval_sending_h_prev;
    }

    char tempo[10];
    const char nanStr[4] = "nan";
    if (getSubString(relevant_data, tempo, table[2] + 1, table[3] - 1) == 0) {
    } else {
    } //error
    if (strcmp(tempo, nanStr) == 0) {
        total_capacity = atoi((const char *) tempo);
    }
    if (getSubString(relevant_data, tempo, table[3] + 1, strlen(relevant_data)) == 0) {
    } else {
    } //error

    if (strcmp(tempo, nanStr) == 0) {
        energyIn = atoi((const char *) tempo);
        gauge = energyIn*total_capacity;
    }
}

// Used to sp
void extract_data_msg(char message_received[]) {
    #define MSG_RECEIVED_LENGTH 50
    char receivedMsg[MSG_RECEIVED_LENGTH];
    char ch = 34; //character ' " '
    int i = 0;
    int ind = 0;
    int indexes_quotation_mark[4];

    while (message_received[i] != '\0') {
        if (message_received[i] == ch) {
            indexes_quotation_mark[ind] = i;
            ind++;
        }
        i++;
    }
    char topic[10];
    char relevant_info[15];

    if (getSubString(receivedMsg, topic, indexes_quotation_mark[0] + 1, indexes_quotation_mark[1] - 1) == 0) {
    } else {
    } //error
    if (getSubString(receivedMsg, relevant_info, indexes_quotation_mark[2] + 1, indexes_quotation_mark[3] - 1) == 0) {
    } else {
    } //error

    extract_relevant_data(relevant_info);
}
*/