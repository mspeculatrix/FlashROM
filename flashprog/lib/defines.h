#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <avr/io.h>

#define OUTPUT 0b00000000		// for setting data port direction
#define INPUT  0b11111111

#define DBON setLED(ALERT_LED, ON)
#define DBOFF setLED(ALERT_LED, OFF)

#define BAUDRATE 9600
#define NEWLINE 10
#define CR 13

#define CHUNKSIZE 64

#define FL_WE PC1				// ATMEGA pins used for control/addressing of
#define FL_OE PC2				// flash chip
#define FL_CE PC3

#define R_WE PD7				// ATMEGA pins used for control/addressing of
#define R_OE PD6				// SRAM chip
#define R_CE PD5

#define A16 PE2  				// for highest addresses

#define MCP_RESET PC6

#define CMD_BUF_LEN 32
#define MAX_MSG_TRIES 4

#define S1_LED PB1
#define S2_LED PB2
#define ACT_LED PB1				// Aliases
#define ALERT_LED PB2

#endif
