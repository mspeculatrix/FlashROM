#ifndef __FLASH_FUNCTIONS_H__
#define __FLASH_FUNCTIONS_H__

#include <avr/io.h>
#include <util/delay.h>
#include "smd_std_macros.h"
#include "smd_avr_mcp23008.h"
#include "smd_avr_mcp23017.h"
#include "defines.h"
#include "functions.h"

extern SMD_MCP23008 dataport;
extern SMD_MCP23017 addrport;

#define FLASH_SECTOR_SIZE 4096

// PROTOTYPES

void flashByteWrite(uint16_t address, uint8_t value);
void flashWrite(uint16_t address, uint8_t value);
uint8_t readFlash(uint32_t address);
void sectorErase(uint16_t startAddress);

// FUNCTIONS

// This is the main function to call to write a byte to memory. Makes multiple
// calls to flashWrite below.
void flashByteWrite(uint16_t address, uint8_t value) {
	flashWrite(0x5555, 0xAA);
	flashWrite(0x2AAA, 0x55);
	flashWrite(0x5555, 0xA0);
	flashWrite(address, value);
}

// Send a value to an address
void flashWrite(uint16_t address, uint8_t value) {
	setPin(&PORTC, FL_CE, LOW);
	addrport.setWord(address);
	dataport.setByte(value);
	setPin(&PORTC, FL_WE, LOW); 	// latches address
	_delay_us(20); 					// pause for effect
	setPin(&PORTC, FL_WE, HIGH);
	setPin(&PORTC, FL_CE, HIGH);
}

uint8_t readFlash(uint32_t address) {
	uint8_t value = 0;
	addrport.setWord(address); 		// set address bus
	setPin(&PORTC, FL_CE, LOW);
	setPin(&PORTC, FL_OE, LOW);
	_delay_us(20);
	value = dataport.readByte();
	setPin(&PORTC, FL_OE, HIGH);
	setPin(&PORTC, FL_CE, HIGH);
	return value;
}

void sectorErase(uint16_t startAddress) {
	flashWrite(0x5555, 0xAA);
	flashWrite(0x2AAA, 0x55);
	flashWrite(0x5555, 0x80);
	flashWrite(0x5555, 0xAA);
	flashWrite(0x2AAA, 0x55);
	flashWrite(startAddress, 0x30);
	_delay_us(25);
}

#endif
