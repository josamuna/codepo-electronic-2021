/* 
 * File: thingstream.h  
 * Author: Cécile Castiaux
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  

/* Defining enable & disable functions for the UART */
#define THINGSTREAM_UART_ENABLE()        U1MODEbits.UARTEN=1;U1STAbits.UTXEN=1;
#define THINGSTREAM_UART_DISABLE()       U1STAbits.UTXEN=0;U1MODEbits.UARTEN=0;

#define LED_ON() _LATB11 = 1;
#define LED_OFF() _LATB11 = 0;

int16_t tsSendInstruction(char cmd[], int cmd_length, char* echo, int echo_length);
int16_t tsReceiveMsg(char* response);
/*
void write_string(char *dest, char text[]);
int  getSubString(char *source, char *target,int from, int to);
void extract_relevant_data(char* relevant_data, int *mode, int *interval_bat_s, int *interval_sending_h);
void extract_mode_and_intervals(char message_received[], int *mode, int *interval_bat_s, int *interval_sending_h);
void extract_lat_long(char *latitude, char *longitude, char echo_gnss[]);
*/
#endif	/* THINGSTREAM_H */








