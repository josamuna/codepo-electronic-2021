/* 
 * File:   main.c
 * Author: nicolaswallemacq
 * check this link for fsm template https://www.microchip.com/forums/m520736.aspx
 * Created on May 2, 2021, 3:59 PM
 * This is the main function for the monitoring of the Zendure battery. 
 */

/* Importing libraries specific for the hardware & the microcontroller*/
#include "xc.h"
#include "uart1.h"
#include "init.h"
#include "libpic30.h"
#include "adc.h" 
#include <libq.h>
#include <dsp.h>
/* Importing other libraries */
#include "thingstream.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h> 
#include <stdbool.h>
#include <time.h>
#include <ctype.h>


/* Defining the global variables */
char topic_publish[9] = "DtW\0";
char caseid[9] = "FICT8C79\0" ; 

#define PUBLISH_CMD_LENGTH 250

#define CREATE_CMD_LENGTH  13
char createCmd[CREATE_CMD_LENGTH] = "AT+IOTCREATE\n";  // 13 char
//char* echo_create_success = "+IOTCREATE: SUCCESS";
#define CREATE_ECHO_LENGTH 19
char echo_create_success[CREATE_ECHO_LENGTH] = "+IOTCREATE: SUCCESS";

#define CONNECT_CMD_LENGTH  19
char connectCmd[CONNECT_CMD_LENGTH] = "AT+IOTCONNECT=true\n";
#define CONNECT_ECHO_LENGTH 20
char echo_connect_success[CONNECT_ECHO_LENGTH] = "+IOTCONNECT: SUCCESS";

#define ASKCONNECT_CMD_LENGTH  15
char askconnectCmd[ASKCONNECT_CMD_LENGTH] = "AT+IOTCONNECT?\n";
#define ASKCONNECT_ECHO_LENGTH 17
char echo_askconnect_success[ASKCONNECT_ECHO_LENGTH] = "+IOTCONNECT: TRUE";

#define TEST_CMD_LENGTH   11
char testCmd[TEST_CMD_LENGTH] = "AT+IOTTEST\n";
#define TEST_ECHO_LENGTH
char echo_iottest[10] = "+IOTTEST\n";

#define GNSSpowerON_CMD_LENGTH 16
char GNSSpowerONCmd[GNSSpowerON_CMD_LENGTH]     = "AT+IOTCGNSPWR=1\n";
#define GNSSpowerON_ECHO_LENGTH 20
char echo_GNSS_on_success[GNSSpowerON_ECHO_LENGTH]= "+IOTCGNSPWR: SUCCESS";

#define GNSSpowerOFF_CMD_LENGTH 16
char GNSSpowerOFFCmd[GNSSpowerOFF_CMD_LENGTH]     = "AT+IOTCGNSPWR=0\n";
#define GNSSpowerOFF_ECHO_LENGTH 20
char echo_GNSS_off_success[GNSSpowerOFF_ECHO_LENGTH]= "+IOTCGNSPWR: SUCCESS";

#define GNSS_CMD_LENGTH 14
char GNSSCmd[GNSS_CMD_LENGTH]     = "AT+IOTCGNSINF\n";
#define GNSS_ECHO_LENGTH 16
char echo_GNSS_success[GNSS_ECHO_LENGTH]= "+IOTCGNSINF: 1,1";

#define DISCONNECT_CMD_LENGTH  17
char disconnectCmd[DISCONNECT_CMD_LENGTH]   = "AT+IOTDISCONNECT\n";
#define DISCONNECT_ECHO_LENGTH 23
char echo_disconnect_success[DISCONNECT_ECHO_LENGTH] = "+IOTDISCONNECT: SUCCESS";

#define DESTROY_CMD_LENGTH  14
char destroyCmd[DESTROY_CMD_LENGTH]         = "AT+IOTDESTROY\n";
#define DESTROY_ECHO_LENGTH 20
char echo_destroy_success[DESTROY_ECHO_LENGTH] = "+IOTDESTROY: SUCCESS";

#define SUBSCRIBE_CMD_LENGTH 29
char subscribeCmd[SUBSCRIBE_CMD_LENGTH];//     = "AT+IOTSUBSCRIBE=\"FICT8C79\",2\n";

#define SUBSCRIBE_ECHO_LENGTH 22
char echo_subscribe_success[SUBSCRIBE_ECHO_LENGTH] = "+IOTSUBSCRIBE: SUCCESS";

#define UNSUBSCRIBE_CMD_LENGTH 24
char unsubscribeCmd[UNSUBSCRIBE_CMD_LENGTH]     = "AT+IOTUNSUBSCRIBE=\"WtD\"\n";
#define UNSUBSCRIBE_ECHO_LENGTH 24 //check
char echo_unsubscribe_success[UNSUBSCRIBE_ECHO_LENGTH] = "+IOTUNSUBSCRIBE: SUCCESS";

#define RECEIVE_ECHO_LENGTH 11
char echo_receive_success[RECEIVE_ECHO_LENGTH] = "+IOTRECEIVE";
#define PUBLISH_ECHO_LENGTH 20
char echo_publish_success[PUBLISH_ECHO_LENGTH] = "+IOTPUBLISH: SUCCESS";
#define PUBLISH_ECHO_LENGTH 20

#define MSG_RECEIVED_LENGTH 50
char msg_received[MSG_RECEIVED_LENGTH];

#define GNSS_INFO_LENGTH 57
char echo_GNSS[GNSS_INFO_LENGTH];
#define LATITUDE_LENGTH 10
#define LONGITUDE_LENGTH 10
char latitude[LATITUDE_LENGTH];
char longitude[LONGITUDE_LENGTH];

/* Functions for switching the led on and off  */
#define LED_ON() _LATB11 = 1;
#define LED_OFF() _LATB11 = 0;


/* Number of attempts (for several instructions) */
#define MAX_TRY     3
/* Number of seconds between the different LED flashes*/
#define TIME_LED_SECONDS 9


/* Defining the time variables */
clock_t t1_bat, t2_bat;
float dt_bat=0;
float total_time_since_last_transmission = 0; //total time that will be the sum of the dt_bat computed
float clock_factor = 3685001.4;
int counter_led = 0;


/* Parametric variables (can be modified via the web platform by sending a specific value in the payload of a message sent to this device) */
int mode = 2, interval_bat_s = 1, interval_sending_h = 0;
/* Ranges of values that those parametric variables can take */
int mode_lower_bound = 1, mode_upper_bound = 3;
int interval_bat_s_lower_bound = 1, interval_bat_s_upper_bound = 60;
int interval_sending_h_lower_bound = 1, interval_sending_h_upper_bound = 168;

/* Variables for the battery monitoring */
int bool_alert_percentage = 0;
float last_percentage = 100;
float percentage = 100;
float voltage_out = 0;
float current_out = 0; 
float voltage_in = 0;
float current_in = 0;
float resistance_out = 0.04436;
float resistance_in =  0.04436;
float total_capacity = 26800; //number of mAh in the zendure battery (fully charged)
float gauge = 26800; // The battery is fully charged
float gain_in = 45; // amplification ==> diviser dans les courants
float gain_out = 15; // amplification ==> diviser dans les courants
float avg_current_out = 3;
float autonomy = 0;

/* Wait function (in seconds) */
void wait(int time){ //time in seconds
    uint16_t limit = time*10;
    uint16_t count = 0;
    for (count=0; count<limit ; ++count) {
        __delay_ms(100);
    }
}

/* Used to write a string into a char array*/
void write_string(char *dest, char text[]){
    int len = strlen(text);
    int i=0;
    for (i=0; i<len;i++){
        dest[i] = text[i];
    }
    dest[len]= '\0';
}

/* Used to extract a substring located between the indexes 'from' and 'to' from 'source' and store it into 'target'. */
int  getSubString(char *source, char *target,int from, int to)
{
	int length=0;
	int i=0,j=0;
    length = strlen(source);
	if(from<0 || from>length){
		printf("Invalid \'from\' index\n");
		return 1;
	}
	if(to>length){
		printf("Invalid \'to\' index\n");
		return 1;
	}	
    for(i = from; i<=to; i++){
        target[j]=source[i];
        j++;
    }
    target[j]='\0';
	return 0;	
}

/* Used to extract the relevant data (mode, interval_bat_s, interval_sending_h) from the message received from the web */
void extract_relevant_data(char* relevant_data){
    //int table[3];
    int table[5];
    int mode_prev = mode;
    int interval_bat_s_prev = interval_bat_s;
    int interval_sending_h_prev = interval_sending_h;
    int temp = 1;
    int a =0;
    int len = strlen(relevant_data);
    int i=0;
    for (i = 0; i < len ;i++){
        if (relevant_data[i] == ',' ) {
            table[a] = i;
            a++;
        }
    }
    
    temp = atoi(&relevant_data[0]);
    if (temp>=mode_lower_bound && temp<=mode_upper_bound){mode = temp;}
    else{mode = mode_prev;}

    temp = atoi(&relevant_data[table[0]+1]);
    if (temp>=interval_bat_s_lower_bound && temp <= interval_bat_s_upper_bound){interval_bat_s = temp;}
    else{interval_bat_s=interval_bat_s_prev;}

    temp = atoi(&relevant_data[table[1]+1]);
    if (temp>=interval_sending_h_lower_bound && temp <= interval_sending_h_upper_bound){interval_sending_h = temp;}
    else{interval_sending_h=interval_sending_h_prev;}
    
    char tempo[10];
    if(getSubString(relevant_data,tempo,table[2]+1,table[3]-1)==0){}
    else{} //error
    if (tempo != "nan"){
        total_capacity = atoi(&tempo);
    }
    if(getSubString(relevant_data,tempo,table[3]+1,strlen(relevant_data))==0){}
    else{} //error

    if (tempo != "nan"){
        percentage = atoi(&tempo);
        gauge = percentage*total_capacity;
    }
}

/* Used to sp*/
void extract_data_msg(char message_received[]){
    char ch = 34; //character ' " '
    int i = 0;
    int ind=0;
    int indexes_quotation_mark[4];
    
    while(message_received[i] != '\0'){
  		if(message_received[i] == ch){
            indexes_quotation_mark[ind] = i;
            ind++;
 		}
 		i++;
	}
    char topic[10];
    char relevant_info[15];

    if(getSubString(msg_received,topic,indexes_quotation_mark[0]+1,indexes_quotation_mark[1]-1)==0){}
    else{} //error
    if(getSubString(msg_received,relevant_info,indexes_quotation_mark[2]+1,indexes_quotation_mark[3]-1)==0){}
    else{} //error

    extract_relevant_data(relevant_info);
}

/* Used to extract the latitude and longitude from the full information received from the GNSS antenna and stored in 'echo_gnss' */
void extract_lat_long(char *lat, char *longi, char echo_gnss[]){
    char ch = 44; // character ',' in ASCII
    int i = 0;
    int ind=0;
    int indexes[5]; 
    while(echo_gnss[i] != '\0')
  	{
  		if(echo_gnss[i] == ch)  
		{
            indexes[ind] = i;
            ind++;
 		}
 		i++;
	}
    if(getSubString(echo_gnss,longi,indexes[2]+1,indexes[3]-1)==0){}
    else{} //printf("Function execution failed!!!\n");
    if(getSubString(echo_gnss,lat,indexes[3]+1,indexes[4]-1)==0){}
    else{} //printf("Function execution failed!!!\n");

}

/* Defining the different states of the finite state machine */
typedef enum {
    AWAKE,
    BATGAUGE,
    ESTABLISHCONNECTION,
    GPSACQUISITION,
    SENDING,
    DESTROYCONNECTION,
} fsmState_t;

/* Main function: implemented as a finite state machine*/
int main(void) {     
    fsmState_t fsmState;
    /* Serial & Pins Setup */
    pinInit();
    adcInit(ADC_MANUAL_SAMPLING); // Configuration de L'ADC pour un declenchement par adcStart())
    TRISB = 0x07FF;
    _TRISB14 = 1; // FIX TO BE ABLE TO HAVE BOTH THE BAT GAUGE AND THE COMMUNICATION OPERATIONAL
    _TRISB15 = 1; // FIX TO BE ABLE TO HAVE BOTH THE BAT GAUGE AND THE COMMUNICATION OPERATIONAL
    uart1Config(115200, FCY, UART_8N1_NO_HANDSHAKE); 
 
    /* Starting the clock */
    t1_bat = clock(); 
    
    /* Switching the LED on for a few seconds to alert the user that the program is starting */
    LED_ON();
    wait(7); 
    LED_OFF();
    
    /* Starting the finite state machine by entering the AWAKE state */
    fsmState = AWAKE;
    
    while(1){
        switch (fsmState) {
            
            /* AWAKE state: standard state in which we're in. Checks if it is time to measure the current/voltage of the battery
             of if it is time to send a message to the server */
            case AWAKE:
                ClrWdt(); // resets the watchdog timer to 0
                wait(1);
                counter_led+=1;
                if (counter_led>=9){
                    LED_ON();
                    wait(2);//__delay_ms();
                    LED_OFF();
                    counter_led=0;
                }
                t2_bat = clock();
                dt_bat = difftime(t2_bat, t1_bat) /clock_factor;  
                if (dt_bat > interval_bat_s ){
                    total_time_since_last_transmission+=dt_bat;
                    fsmState = BATGAUGE;
                }
                if (total_time_since_last_transmission > interval_sending_h*3600 || bool_alert_percentage){
//                    dummyForBreakpoint=0;
                    total_time_since_last_transmission=0;
                    fsmState = ESTABLISHCONNECTION;
                }
                break;
            
            case BATGAUGE: 
                // Starting conversion for pin AN1
                adcStart(1);
                //Waiting for the conversion to be done
                while (!adcConversionDone()) {}
                // Reading
                voltage_out = ( 3.3*adcRead() ) / gain_out / 1023;
                current_out = voltage_out/resistance_out; 

                // Starting conversion for pin AN0
                adcStart(0);
                //Waiting for the conversion to be done
                while (!adcConversionDone()) {}
                voltage_in =  (3.3*adcRead() ) / gain_in  / 1023;;
                current_in = voltage_in/resistance_in;

                t2_bat = clock(); // second time ref
                
                dt_bat = difftime(t2_bat,t1_bat)/clock_factor;
                
                // Computing the percentage
                gauge = gauge - (current_out*1000)*(dt_bat/3600) + (current_in*1000)*(dt_bat/3600);
                last_percentage = percentage;
                percentage = 100*(gauge/total_capacity);
                
                // Computing the remaining autonomy
                avg_current_out = (avg_current_out + current_out)/2; 
                autonomy = gauge/(avg_current_out*1000);
                
                if(percentage <= 75 && last_percentage > 75){
                    bool_alert_percentage = 1;
                }
                if(percentage <= 50 && last_percentage > 50){
                    bool_alert_percentage = 1;
                }
                if(percentage <= 25 && last_percentage > 25){
                    bool_alert_percentage = 1;
                }
                if(percentage <= 5 && last_percentage > 5){
                    bool_alert_percentage = 1;
                }
                
                /* Reseting to 0 the timer */
                dt_bat=0;
                t1_bat = clock();
                fsmState = AWAKE; // going back to the standard state
                break;

            /* ESTABLISHCONNECTION state: creation & connection to the Thingstream server */
            case ESTABLISHCONNECTION:
                ClrWdt();
                THINGSTREAM_POWER_ON(); // SWITCHING ON THE COMMUNICATION MODULE
                THINGSTREAM_UART_ENABLE(); // SWITCHING ON THE UART CHANNEL
                
                /* Defining the different echos */
                char echo_create[CREATE_ECHO_LENGTH];
                char echo_connect[CONNECT_ECHO_LENGTH];
                char echo_subscribe[SUBSCRIBE_ECHO_LENGTH];
                sprintf(subscribeCmd, "AT+IOTSUBSCRIBE=\"%s\",2\n", caseid) ;
                char echo_unsubscribe[UNSUBSCRIBE_ECHO_LENGTH];
                char echo_askconnect[ASKCONNECT_ECHO_LENGTH];

                /* Defining variables for trying multiple times the AT command */
                int trying_create = 1, trying_connect = 1;
                int nb_attempts_create = 0, nb_attempts_connect = 0;

                /* Creation of the connection to Thingstream */
                while(trying_create){ // tries maximum MAX_TRY times to send the AT command. 
                    wait(2); 
                    tsSendInstruction(createCmd, CREATE_CMD_LENGTH, echo_create, CREATE_ECHO_LENGTH);
                    nb_attempts_create++;
                    int stopTrying = memcmp(echo_create_success, echo_create, sizeof echo_create_success);//returns 0 if successful echo
//                    dummyForBreakpoint=0;
                    if ((stopTrying==0)|| (nb_attempts_create>=MAX_TRY)) {trying_create=0;}
                }
                
                ClrWdt();
                wait(1);
                /* Verifies if the Click is already connected to Thingstream */
                tsSendInstruction(askconnectCmd, ASKCONNECT_CMD_LENGTH, echo_askconnect, ASKCONNECT_ECHO_LENGTH);
                if( memcmp(echo_connect_success, echo_connect, sizeof echo_connect_success) != 0){
                    /* Connects Thingstream because it wasn't already */
                    while(trying_connect){ // tries maximum MAX_TRY times to send the AT command. 
                       wait(2);
                       tsSendInstruction(connectCmd, CONNECT_CMD_LENGTH, echo_connect, CONNECT_ECHO_LENGTH);
                       nb_attempts_connect++;
                       int stopTrying = memcmp(echo_connect_success, echo_connect, sizeof echo_connect_success);//returns 0 if successful echo
//                       dummyForBreakpoint=0;
                       if ((stopTrying==0)|| (nb_attempts_connect>=MAX_TRY)) {trying_connect=0;}
                   }
                }
                
                ClrWdt();

                
                /* Listening for messages sent by the server to this device. A message can only be received 
                  once subscribed to the topic the message was posted on */
                
                tsSendInstruction(subscribeCmd, SUBSCRIBE_CMD_LENGTH, echo_subscribe, SUBSCRIBE_ECHO_LENGTH);
                wait(1);
                tsReceiveMsg(msg_received);
                extract_data_msg(msg_received); // extraction of the relevant data
                wait(1);
                tsSendInstruction(unsubscribeCmd, UNSUBSCRIBE_CMD_LENGTH, echo_unsubscribe, UNSUBSCRIBE_ECHO_LENGTH);
                if (mode==1){ 
                    fsmState=SENDING;
                }
                
                else if (mode==2){
                    fsmState=GPSACQUISITION;
                }
                
                else if (mode==3) { //for mode ECONOMY
                    fsmState=DESTROYCONNECTION;
                }
                
                else {
                    fsmState = DESTROYCONNECTION;
                }
                
                break;
                
            case SENDING:
                ClrWdt();
                /* Defining the publish command string */
                char publishCmd[PUBLISH_CMD_LENGTH];
                /* Defining the echo */
                char echo_publish[PUBLISH_ECHO_LENGTH]; 
                /* Defining variables for trying multiple times the AT command */
                int nb_attempts_sending=0, trying_sending = 1;

                /* Writing the message to be sent to the server */
                int lengthPublishString;
//                dummyForBreakpoint = 0;
                if (mode==1){lengthPublishString = sprintf(publishCmd,"AT+IOTPUBLISH=\"%s\",1,\"{&caseid&: %s, &SOC&: %.2f, &autonomy&: %.2f, &latitude&: hidden, &longitude&: hidden, &mode&: %d, &interval_bat_s&: %d, &interval_sending_h&: %d, &alert_battery&: %d}\",true\n", topic_publish, caseid, percentage, autonomy, mode, interval_bat_s, interval_sending_h, bool_alert_percentage);}
                else if (mode==2){lengthPublishString = sprintf(publishCmd,"AT+IOTPUBLISH=\"%s\",1,\"{&caseid&: %s, &SOC&: %.2f, &autonomy&: %.2f, &latitude&: %s, &longitude&: %s, &mode&: %d, &interval_bat_s&: %d, &interval_sending_h&: %d, &alert_battery&: %d}\",true\n", topic_publish, caseid, percentage, autonomy, latitude, longitude, mode, interval_bat_s, interval_sending_h, bool_alert_percentage);}
                
                /* Publishing the message on the topic specified in publishCmd */
                while(trying_sending){ // tries maximum MAX_TRY times to send the AT command. 
                    wait(2);
                    tsSendInstruction(publishCmd, lengthPublishString, echo_publish, PUBLISH_ECHO_LENGTH);
                    nb_attempts_sending++;
                    int stopTrying = memcmp(echo_publish, echo_publish_success, sizeof echo_publish_success);
                    if ((stopTrying==0) || (nb_attempts_sending>=MAX_TRY)){
                        trying_sending=0;
                    } 
                }
                
               
                fsmState = DESTROYCONNECTION;
                break;

                
            /* DESTROYCONNECTION state: destruction of the connection we have with the Thingstream Server */
            case DESTROYCONNECTION:
                ClrWdt();
                /* Defining the different echos */
                char echo_disconnect[DISCONNECT_ECHO_LENGTH]; 
                char echo_destroy[DESTROY_ECHO_LENGTH];
                 /* Defining variables for trying multiple times the AT command */
                int trying_disconnect = 1, trying_destroy = 1;
                int nb_attempts_disconnect = 0, nb_attempts_destroy = 0;
                
                /* Disconnection with Thingstream */
                while(trying_disconnect){ // tries maximum MAX_TRY times to send the AT command. 
                    wait(2);
                    tsSendInstruction(disconnectCmd, DISCONNECT_CMD_LENGTH, echo_disconnect, DISCONNECT_ECHO_LENGTH);
                    nb_attempts_disconnect++;
                    int stopTrying = memcmp(echo_disconnect_success, echo_disconnect, sizeof echo_disconnect_success);//returns 0 if successful echo
//                    dummyForBreakpoint=0;
                    if ((stopTrying==0)|| (nb_attempts_disconnect>=MAX_TRY)) {trying_disconnect=0;}
                }
                
                ClrWdt();
                
                /* Destroying the connection */
                while(trying_destroy){ // tries maximum MAX_TRY times to send the AT command. 
                    wait(2);
                    tsSendInstruction(destroyCmd, DESTROY_CMD_LENGTH, echo_destroy,DESTROY_ECHO_LENGTH);
                    nb_attempts_destroy++;
                    int stopTrying = memcmp(echo_destroy_success, echo_destroy, sizeof echo_destroy_success);//returns 0 if successful echo
//                    dummyForBreakpoint=0;
                    if ((stopTrying==0)|| (nb_attempts_destroy>=MAX_TRY)) {trying_destroy=0;}
                }

                THINGSTREAM_UART_DISABLE(); // SWITCHING OF THE UART CHANNEL
                THINGSTREAM_POWER_DOWN(); // SWITCHING OFF THE COMMUNICATION MODULE
                bool_alert_percentage = 0;
                fsmState = AWAKE;
                break;
            
            /* GPSACQUISITION state: recovery of the GPS coordinates (latitude and longitude)*/
            case GPSACQUISITION:
                ClrWdt();
                /* Defining the different echos */
                char echo_GNSS_on[GNSSpowerON_ECHO_LENGTH];
                char echo_GNSS_off[GNSSpowerOFF_ECHO_LENGTH];
                 /* Defining variables for trying multiple times the AT command */
                int trying_GNSS_ON = 1, trying_GNSS = 1,trying_GNSS_OFF=1;
                int nb_attempts_GNSS_ON=0, nb_attempts_GNSS=0, nb_attempts_GNSS_OFF=0;
                
                /* Switching on the GPS antenna */
                while(trying_GNSS_ON){
                    wait(1);
                    tsSendInstruction(GNSSpowerONCmd, GNSSpowerON_CMD_LENGTH, echo_GNSS_on, GNSSpowerON_ECHO_LENGTH);
                    nb_attempts_GNSS_ON++;
                    int stopListening = memcmp(echo_GNSS_on, echo_GNSS_on_success, sizeof echo_GNSS_on_success); //returns 0 if successfull echo
//                    dummyForBreakpoint=0;
                    if ((stopListening==0)|| (nb_attempts_GNSS_ON>=MAX_TRY)) {trying_GNSS_ON=0;} 
                }

                wait(40); // wait long enough to have a valid GNSS fix
                
                /* Getting a GNSS fix */
                while(trying_GNSS){ // tries maximum MAX_TRY times to send the AT command.
                    wait(15); // wait 15 seconds more to get more time to get a GNSS fix
                    tsSendInstruction(GNSSCmd, GNSS_CMD_LENGTH, echo_GNSS, GNSS_INFO_LENGTH);
                    nb_attempts_GNSS++;
                    int stopListening = memcmp(echo_GNSS, echo_GNSS_success, sizeof echo_GNSS_success); //returns 0 if successfull echo
                    if ((stopListening==0)|| (nb_attempts_GNSS>=MAX_TRY)) {trying_GNSS=0;} 
                }
                /* Latitude and longitude extraction from the echo received from the Thingstream Click*/
                if (memcmp(echo_GNSS, echo_GNSS_success, sizeof echo_GNSS_success)==0){
                    extract_lat_long(latitude,longitude, echo_GNSS);
                }
                else{ //we didn't get the latitude and longitude 
                    write_string(latitude, "nan"); //nan = not a number
                    write_string(longitude, "nan"); //nan = not a number
                }
                
                ClrWdt();

                wait(1);
                
                /* Switching off the GPS antenna (but never works, the only way to shut it down is to shut down the Click) */
                while(trying_GNSS_OFF){ // tries maximum MAX_TRY times to send the AT command.
                    wait(1);
                    tsSendInstruction(GNSSpowerOFFCmd, GNSSpowerOFF_CMD_LENGTH, echo_GNSS_off, GNSSpowerOFF_ECHO_LENGTH);
                    nb_attempts_GNSS_OFF++;
                    int stopListening = memcmp(echo_GNSS_off, echo_GNSS_off_success, sizeof echo_GNSS_off_success); //returns 0 if successfull echo
//                    dummyForBreakpoint=0;
                    if ((stopListening==0)|| (nb_attempts_GNSS_OFF>=MAX_TRY)) {trying_GNSS_OFF=0;} 
                }
                
                fsmState = SENDING; 
                break;
        }
    }
    return(0);
}

          

