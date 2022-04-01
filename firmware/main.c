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
//#include <math.h>
//#include <float.h> 
//#include <libq.h>
//#include <dsp.h>
//#include <stdbool.h>
//#include <ctype.h>


#define COMM_PERIOD_H       4   //Période d'envoi des messages, en heure
#define SLEEP_PERIOD_SEC    128 // Durée d'un "sleep", en secondes

/* Defining the global variables */
char publishTopic0[] = "FICT8C79-DtW\0";
char publishTopic1[] = "FICT8C80-DtW\0";
char publishTopic2[] = "FICT8C81-DtW\0";
char caseId0[9] = "FICT8C79\0";
char caseId1[9] = "FICT8C80\0";
char caseId2[9] = "FICT8C81\0";
int16_t curTopic = 0;


#define MSG_RECEIVED_LENGTH 50
char receivedMsg[MSG_RECEIVED_LENGTH];

#define LATITUDE_LENGTH 10
#define LONGITUDE_LENGTH 10
char latitude[LATITUDE_LENGTH];
char longitude[LONGITUDE_LENGTH];

/* Parametric variables (can be modified via the web platform by sending a specific value in the payload of a message sent to this device) */
workMode_t workMode = BAT_GPS;
//workMode_t workMode = BAT;

/* Ranges of values that those parametric variables can take */
int mode_lower_bound = 1, mode_upper_bound = 3;
int interval_bat_s_lower_bound = 1, interval_bat_s_upper_bound = 60;
int interval_sending_h_lower_bound = 1, interval_sending_h_upper_bound = 168;

/* Variables for the battery monitoring */
int16_t bool_alert_percentage = 0;
float last_percentage = 100;
uint8_t percentage = 100;
float total_capacity = 26800; //number of mAh in the zendure battery (fully charged)
// Cpacité de la batterie, en mAh
float gauge = 26800; // The battery is fully charged
float avg_current_out = 3;
float autonomy = 0;

void batGaugeFunction(uint16_t batSleepCount);
void awakeFunction(void);
void delaySec(int time);
void write_string(char *dest, char text[]);
int getSubString(char *source, char *target, int from, int to);
void extract_relevant_data(char* relevant_data);
void extract_data_msg(char message_received[]);
void extract_lat_long(char *lat, char *longi, char echo_gnss[]);

/* Defining the different states of the finite state machine */
typedef enum {
    SLEEPING,
    MEASURE,
    ESTABLISHCONNECTION,
    LISTENING,
    GPS_INIT,
    GPS_READ,
    SENDING,
    DESTROY_CONNECTION,
    ERROR
} fsmState_t;


/* Main function: implemented as a finite state machine*/
int main(void) {
    fsmState_t fsmState;
    tsError_t tsError;
    // periods are counted in sleep cycles, determined by the watchdog.  sleep cycle is 128 sec
    uint16_t batSleepThreshold = 1; // number of sleep cycle between measures
    uint16_t commSleepThreshold = (COMM_PERIOD_H * 3600) / SLEEP_PERIOD_SEC; // number of sleep cycle between messages (1h = 28 cycles)
    // sleep cycle counters, initialised to their threshold for immediate action
    uint16_t batSleepCount = batSleepThreshold;
    uint16_t commSleepCount = commSleepThreshold;//sendingSleepThreshold;
    gpsCoord_t gpsCoord;
    uint16_t gpsTry;
    char errStr[] = "201";


    /* Serial & Pins Setup */
    pinInit();
    adcInit(ADC_MANUAL_SAMPLING); // Configuration de L'ADC pour un declenchement par adcStart())
    uart1Config(115200, FCY, UART_8N1_NO_HANDSHAKE);

    /* Switching the LED on for a few seconds to alert the user that the program is starting */
    LED_ON();
    __delay_ms(1000); 
    LED_OFF();

    /* Starting the finite state machine by entering the AWAKE state */
    fsmState = SLEEPING;
    percentage = 10;
    

    while (1) {
        switch (fsmState) {
                /**
                 * Sleeping state: 
                 * check, in decreasing priority if:
                 *  - it's time to measure
                 *  - it's time to send message
                 * if not, start a new sleep cycle
                 */
            case SLEEPING:
                // transitions
                if (batSleepCount >= batSleepThreshold) {
                    fsmState = MEASURE;
                }
                else if (commSleepCount >= commSleepThreshold) {
                    fsmState = ESTABLISHCONNECTION;
                }
                else {
                    // actions
                    Sleep();
                    batSleepCount++;
                    commSleepCount++;
                }
                break;

            case MEASURE:
                batGaugeFunction(batSleepCount);
                batSleepCount = 0;
                if (commSleepCount >= commSleepThreshold) {
                    fsmState = ESTABLISHCONNECTION;
                }
                else {
                    fsmState = SLEEPING;
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
/*                tsError = tsSubscribe(caseId);
                if (tsError == NO_ERROR) {
                    tsError = tsReceiveMsg(receivedMsg, MSG_RECEIVED_LENGTH);
                    if (tsError == NO_ERROR) {
                        dummy = 41;
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
                curTopic = 0;
                if (curTopic == 0) {
                    tsError = tsPublish(publishTopic0, caseId0, percentage, autonomy, gpsCoord, workMode, batSleepThreshold*SLEEP_PERIOD_SEC, COMM_PERIOD_H, bool_alert_percentage);
                    curTopic++;
                } else if (curTopic == 1) {
                    tsError = tsPublish(publishTopic1, caseId1, percentage, autonomy, gpsCoord, workMode, 2, 5, bool_alert_percentage);
                    curTopic++;
                } else {
                    tsError = tsPublish(publishTopic2, caseId2, percentage, autonomy, gpsCoord, workMode, 2, 5, bool_alert_percentage);
                    curTopic = 0;
                }
                percentage++;
                if (percentage > 99) {
                    percentage = 10;
                }

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
                THINGSTREAM_POWER_DOWN(); // SWITCHING OFF THE COMMUNICATION MODULE
                bool_alert_percentage = 0;
                commSleepCount = 0;
                fsmState = SLEEPING;
                break;

            case GPS_INIT:
                tsError = tsGNSSpowerOn();
                if (tsError == NO_ERROR) {
                    fsmState = GPS_READ;
                } else {
                    strcpy(gpsCoord.longitude, errStr);
                    strcpy(gpsCoord.latitude, errStr);
                    fsmState = SENDING;
                }
                gpsTry = 0;
                Sleep();
                batSleepCount++;
                commSleepCount++;
                break;
                
                
            /* GPSACQUISITION state: recovery of the GPS coordinates (latitude and longitude)*/
            case GPS_READ:
                if (gpsTry++ < 1) {
                    if (tsGNSSCmd(&gpsCoord) == NO_ERROR) {
                        // tsGNSSpowerOff();    // creates an "unespected error" from the click board
                        fsmState = SENDING;
                    } else {
                        Sleep();
                        batGaugeFunction(batSleepCount);
                        batSleepCount = 0;
                        commSleepCount++;
                    }
                }
                else {
                    //tsGNSSpowerOff(); generates an error from the click board
                    fsmState = SENDING;
                }
                break;
        }
    }
    return (0);
}


#define LOAD_GAIN       (51*0.033)  // La résistance de mesure vaut 33mOhm et l'amplificatuer différentiel a un gain de 51
#define CHARGE_GAIN     (15*0.033)  // La résistance de mesure vaut 33mOhm et l'amplificatuer différentiel a un gain de 15
#define CHARGE_CHANNEL  0           // la mesure de Iload se fait sur AN0
#define LOAD_CHANNEL    1           // la mesure de Iload se fait sur AN1

void batGaugeFunction(uint16_t batSleepCount) {
    float loadCurrent;
    float chargeCurrent;
    
    // Mesure de Iload
    adcStart(LOAD_CHANNEL);
    //Waiting for the conversion to be done
    while (!adcConversionDone());
    // Reading
    loadCurrent = LOAD_GAIN * (3.3/1023) * adcRead();

    // SMesrue de Icharge
    adcStart(CHARGE_CHANNEL);
    //Waiting for the conversion to be done
    while (!adcConversionDone());
    chargeCurrent = CHARGE_GAIN * (3.3/1023) * adcRead();

    // Computing the percentage
    // La tension de charge est 7V, on applique donc un facteur 7V/5V au courant de charge
    gauge = gauge + (chargeCurrent*3.5 - loadCurrent) * (batSleepCount * SLEEP_PERIOD_SEC) / 3.6;
    last_percentage = percentage;
    percentage = 100 * (gauge / total_capacity);

    // Computing the remaining autonomy
    avg_current_out = (avg_current_out + loadCurrent) / 2;
    autonomy = gauge / (avg_current_out * 1000);

    if (percentage <= 75 && last_percentage > 75) {
        bool_alert_percentage = 1;
    }
    if (percentage <= 50 && last_percentage > 50) {
        bool_alert_percentage = 1;
    }
    if (percentage <= 25 && last_percentage > 25) {
        bool_alert_percentage = 1;
    }
    if (percentage <= 5 && last_percentage > 5) {
        bool_alert_percentage = 1;
    }
}

/* Wait function (in seconds) */
void delaySec(int time) { //time in seconds
    uint16_t limit = time * 10;
    uint16_t count = 0;
    for (count = 0; count < limit; ++count) {
        __delay_ms(100);
    }
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


// Used to extract the relevant data (mode, interval_bat_s, interval_sending_h) from the message received from the web 
void extract_relevant_data(char* relevant_data) {
    //int table[3];
    int table[5];
    int mode_prev = mode;
    int interval_bat_s_prev = interval_bat_s;
    int interval_sending_h_prev = interval_sending_h;
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
        mode = temp;
    } else {
        mode = mode_prev;
    }

    temp = atoi(&relevant_data[table[0] + 1]);
    if (temp >= interval_bat_s_lower_bound && temp <= interval_bat_s_upper_bound) {
        interval_bat_s = temp;
    } else {
        interval_bat_s = interval_bat_s_prev;
    }

    temp = atoi(&relevant_data[table[1] + 1]);
    if (temp >= interval_sending_h_lower_bound && temp <= interval_sending_h_upper_bound) {
        interval_sending_h = temp;
    } else {
        interval_sending_h = interval_sending_h_prev;
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
        percentage = atoi((const char *) tempo);
        gauge = percentage*total_capacity;
    }
}

// Used to sp
void extract_data_msg(char message_received[]) {
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