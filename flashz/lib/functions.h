#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include <avr/io.h>
#include <util/delay.h>
#include "defines.h"
#include "smd_avr_serial4809.h"

extern SMD_AVR_Serial4809 serial;

// PROTOTYPES
bool checkForMessage(const char* msg, char* buf);
void clearBuf(char* buf, uint8_t len);
void disableAddressBusCtrl();
void disableControlSignals();
void enableAddressBusCtrl();
void enableControlSignals();
uint8_t getCommand(char* buf);
uint16_t getWord();
void resetSystem();
void sendWord(uint16_t word);
void setAddress(uint16_t addr);

// Look for a specific incoming message. Wrapper to getCommand()
// Returns flase if it doesn't get what it's looking for.
bool checkForMessage(const char* msg, char* buf) {
	bool error = false;
	getCommand(buf);
	if (!strcmp(buf, msg) == 0) {
		error = true;
	}
	return error;
}

// Clear any buffer you like by writing zeros to it.
void clearBuf(char* buf, uint8_t len) {
	for (uint8_t i = 0; i < len; i++) {
		buf[i] = 0;
	}
}

// Set the address bus pins as inputs. This effectively makes them high
// impendance so they won't upset things when the Zolatron resumes normal
// operations.
void disableAddressBusCtrl() {
	PORTC.DIR = 0; // PORTC - A0-A5
	PORTE.DIR = 0; // PORTE - A12-A15
	PORTF.DIR = 0; // PORTF - A6-A11
}

// Make control signal pins inputs. As with the address bus, this effectively
// isolates them from the Zolatron.
// We don't touch pins 0 and 1 because those are for the serial port.
void disableControlSignals() {
	CTRL_PORT.DIRCLR = CTRL_PORT_MASK;
}

// Set address pins as outputs
void enableAddressBusCtrl() {
	PORTC.DIR = 0b00111111; // PORTC - A0-A5
	PORTF.DIR = 0b00111111; // PORTF - A6-A11
	PORTE.DIR = 0b00001111; // PORTE - A12-A15
}

// Make control signal pins outputs & set high.
// We don't touch pins 0 and 1 because those are the serial port.
void enableControlSignals() {
	CTRL_PORT.DIRSET = CTRL_PORT_MASK;
	CTRL_PORT.OUTSET = CTRL_PORT_MASK;
}

// Checks for characters coming in over serial and adds them to the given
// command buffer. It's used for commands only.
// Returns the number of chars received, although I don't think I do anything
// with that information right now.
// Returns when it encounters a linefeed or the buffer is full.
uint8_t getCommand(char* buf) {
	clearBuf(buf, CMD_BUF_LEN);
	bool recvd = false;
	uint8_t idx = 0;
	uint8_t inChar = 0;
	while (!recvd && idx < CMD_BUF_LEN) {
		if (serial.readByte(&inChar)) {
			if (inChar == NEWLINE && idx > 0) {
				buf[idx] = 0;	// terminate
				recvd = true;
			} else if (inChar == CR) {
				// ignore carriage returns
			} else {
				buf[idx] = inChar;
				idx++;
			}
		}
	}
	return idx;
}

// Retrieve two bytes from the serial input and return as uint16_t integer.
// Blocking. It won't return until it has received two bytes.
uint16_t getWord() {
	uint16_t word = 0;
	uint8_t byteCount = 2; // because MSB first
	uint8_t wordBuf[2];
	while (byteCount > 0) {
		if (serial.inWaiting()) {
			byteCount--;
			wordBuf[byteCount] = serial.getByte();
		}
	}
	word = (wordBuf[1] << 8) + wordBuf[0];
	return word;
}

// Pulse the reset pin (which is attached to the Zolatron's system
// reset line) low.
void resetSystem() {
	PORTF.DIRSET = SYS_RES; // set PF6 as output
	PORTF.OUTCLR = SYS_RES; // Take low
	_delay_ms(50);
	PORTF.OUTCLR = SYS_RES; // Take high
	PORTF.DIRCLR = SYS_RES; // set PF6 as input
}

// Set a 16-bit address on the address pins. Assumes that
// enableAddressBusCtrl() has already been called.
void setAddress(uint16_t addr) {
	// Start with the lowest 6 bits of the address. We'll use a mask
	// to ensure we use only those bits.
	uint8_t mask = 0b00111111;
	PORTC.OUT = uint8_t(addr) & mask;
	// Now use the next 6 bits, by shifting out the ones we've already used,
	// with the same mask
	addr = addr >> 6;
	PORTF.OUT = uint8_t(addr) & mask;
	// Finally, shift out the used 6 bits. But of those that are getting
	// shifted in, we need only four, so let's adjust the mask.
	addr = addr >> 6;
	mask = 0b00001111;
	PORTE.OUT = uint8_t(addr) & mask;
}

// Send a 16-bit integer, MSB-first.
void sendWord(uint16_t word) {
	serial.sendByte((uint8_t)(word >> 8));		// MSB
	serial.sendByte((uint8_t)(word & 0x00FF));	// LSB
}

#endif
