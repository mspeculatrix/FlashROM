#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <avr/io.h>

#define OUTPUT 0b00000000		// for setting data port direction
#define INPUT  0b11111111
#define NEWLINE 10

#define FL_WE PB0				// ATMEGA pins used for control/addressing of
#define FL_OE PB1				// flash chip
#define FL_CE PB2

#define R_WE PC2				// ATMEGA pins used for control/addressing of
#define R_OE PC1				// SRAM chip
#define R_CE PC0

#define MCP_RESET PC3

#define CMD_BUF_LEN 32
#define MAX_MSG_TRIES 4

#define DEBUG_LED PD7

#endif
