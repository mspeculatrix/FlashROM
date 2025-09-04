#ifndef __RAM_FUNCTIONS_H__
#define __RAM_FUNCTIONS_H__

#include <avr/io.h>
#include <util/delay.h>
#include "smd_std_macros.h"
#include "smd_avr_mcp23008.h"
#include "smd_avr_mcp23017.h"
#include "defines.h"
#include "functions.h"

extern SMD_MCP23008 dataport;
extern SMD_MCP23017 addrport;

uint8_t readRAM(uint16_t addr) {
	uint8_t data = 0;
	dataport.setIODIR(INPUT); 		// Set data port to input
	addrport.setOutput(MCP23017_PORTA, (uint8_t)(addr & 0x00FF)); 	// set addr
	addrport.setOutput(MCP23017_PORTB, (uint8_t)(addr >> 8)); 		// set addr
	_delay_us(1);					// pause
	setPin(&PORTC, R_CE, LOW); 		// Enable /R_CE
	setPin(&PORTC, R_OE, LOW); 		// Enable /R_OE
	_delay_us(10);					// pause
	data = dataport.readInput();	// Read data port
	setPin(&PORTC, R_OE, HIGH);		// Disable /R_OE
	setPin(&PORTC, R_CE, HIGH);		// Disable /R_CE
	return data;
}

void writeRAM(uint16_t addr, uint8_t data) {
	dataport.setOutput(data); 		// Set data on dataport
	addrport.setOutput(MCP23017_PORTA, (uint8_t)(addr & 0x00FF)); 	// set addr
	addrport.setOutput(MCP23017_PORTB, (uint8_t)(addr >> 8)); 		// set addr
	// _delay_us(1);					// pause
	setPin(&PORTC, R_CE, LOW); 		// Enable /R_CE
	setPin(&PORTC, R_WE, LOW); 		// Enable /R_WE
	_delay_us(10);					// pause
	setPin(&PORTC, R_WE, HIGH);		// Disable /R_WE
	setPin(&PORTC, R_CE, HIGH);		// Disable /R_CE
}


#endif
