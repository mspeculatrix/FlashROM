#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__


#include <avr/io.h>
#include <util/delay.h>
#include "defines.h"

// PROTOTYPES
void clearBuf(char* buf, uint8_t len);
void disableAddressBusCtrl();
void disableControlSignals();
void enableAddressBusCtrl();
void enableControlSignals();
void resetSystem();
void setAddress(uint16_t addr);

// Clear any buffer you like by writing zeros to it.
void clearBuf(char* buf, uint8_t len) {
	for (uint8_t i = 0; i < len; i++) {
		buf[i] = 0;
	}
}

void disableAddressBusCtrl() {
	PORTC.DIR = 0; // PORTC - A0-A5
	PORTE.DIR = 0; // PORTE - A12-A15
	PORTF.DIR = 0; // PORTF - A6-A11
}

void disableControlSignals() {
	// Make control signal pins inputs
	// We don't touch pins 0 and 1 because those are the serial port
	PORTA.DIRCLR = CTRL_PORT_MASK;
	// for (uint8_t i = 2; i < 8; i++) {
	// 	PORTA.DIRCLR = (1 << i);
	// }
}

void enableAddressBusCtrl() {
	// Set address pins as outputs
	PORTC.DIR = 0b00111111; // PORTC - A0-A5
	PORTF.DIR = 0b00111111; // PORTF - A6-A11
	PORTE.DIR = 0b00001111; // PORTE - A12-A15
}

void enableControlSignals() {
	// Make control signal pins outputs & set high
	// We don't touch pins 0 and 1 because those are the serial port
	// PORTA.DIRSET = CTRL_PORT_MASK;
	// PORTA.OUTSET = CTRL_PORT_MASK;
	for (uint8_t i = 2; i < 8; i++) {
		PORTA.DIRSET = (1 << i);
		PORTA.OUTSET = (1 << i);
	}
}

void resetSystem() {
	PORTF.DIRSET = SYS_RES; // set PF6 as output
	PORTF.OUTCLR = SYS_RES; // Take low
	_delay_ms(50);
	PORTF.OUTCLR = SYS_RES; // Take high
	PORTF.DIRCLR = SYS_RES; // set PF6 as input
}

void setAddress(uint16_t addr) {
	uint8_t mask = 0b00111111;
	PORTC.OUT = uint8_t(addr) & mask;
	addr = addr >> 6;
	PORTF.OUT = uint8_t(addr) & mask;
	// Need to handle PF6 which is reset - actually this might be okay
	addr = addr >> 6;
	mask = 0b00001111;
	PORTE.OUT = uint8_t(addr) & mask;
}

#endif
