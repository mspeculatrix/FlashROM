/* --- Macros for flashz --- */

#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <avr/io.h>

// #define SERIAL_BAUDRATE 38400UL
#define SERIAL_BAUDRATE 9600UL

#define DATA_PORT_OUTPUT PORTD.DIR = 0xFF
#define DATA_PORT_INPUT PORTD.DIR = 0

#define OUTPUT 0b00000000		// for setting data port direction
#define INPUT  0b11111111

#define NEWLINE 10
#define CR 13

#define CHUNKSIZE 64

// The following signals are all on PORTA
#define CPU_RDY     PIN2_bm
#define CPU_BE      PIN3_bm
#define SYS_PHI2    PIN4_bm
#define SYS_READ_EN PIN5_bm
#define FL_OE       PIN5_bm // alias
#define SYS_ROM_EN  PIN6_bm
#define FL_CE       PIN6_bm // alias
#define FL_WE       PIN7_bm

// This is on PORTF
#define SYS_RES     PIN6_bm

#define CMD_BUF_LEN 32
#define MAX_MSG_TRIES 4

#endif
