
#ifndef __AVR_ATmega4809__
#define __AVR_ATmega4809__
#endif

#ifndef F_CPU
#define F_CPU 20000000UL // 20 MHz unsigned long
#endif

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "lib/defines.h"
#include "lib/smd_avr_serial4809.h"
#include "lib/functions.h"
#include "lib/flash_functions.h"



/*******************************************************************************
*****   GLOBALS                                                            *****
*******************************************************************************/

uint16_t dataSize = 0;
uint8_t dataChunk[CHUNKSIZE];

SMD_AVR_Serial4809 serial = SMD_AVR_Serial4809(SERIAL_BAUDRATE);

/*******************************************************************************
*****   FUNCTIONS                                                          *****
*******************************************************************************/

// PROTOTYPES
bool checkForMessage(const char* msg, char* buf);
uint8_t getCommand(char* buf);

// Look for a specific incoming message. Wrapper to getCommand()
bool checkForMessage(const char* msg, char* buf) {
	bool error = false;
	getCommand(buf);
	if (!strcmp(buf, msg) == 0) {
		error = true;
	}
	return error;
}

// Checks for characters coming in over serial and adds them to the given
// command buffer. It's used for commands only.
// Returns the number of chars received, although I don't think I do anything
// with that information.
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

// Send a 16-bit value across the serial as two bytes, MSB-first.
void sendWord(uint16_t word) {
	serial.sendByte((uint8_t)(word >> 8));	// MSB
	serial.sendByte((uint8_t)(word & 0x00FF));	// LSB
}

/* =============================================================================
   =====   MAIN                                                            =====
   ===========================================================================*/
int main(void) {
	/**   SETUP			 											         **/

	CCP = CCP_IOREG_gc;		// Unlock protected registers
	CLKCTRL.MCLKCTRLB = 0;  // No prescaling, full main clock frequency

	// We're using the 40-pin DIP version of the 4809, so need to configure
	// the non-existent pins, PB0..5, PC6, PC7.
	PORTB.PIN0CTRL = PORT_PULLUPEN_bm;
	PORTB.PIN1CTRL = PORT_PULLUPEN_bm;
	PORTB.PIN2CTRL = PORT_PULLUPEN_bm;
	PORTB.PIN3CTRL = PORT_PULLUPEN_bm;
	PORTB.PIN4CTRL = PORT_PULLUPEN_bm;
	PORTB.PIN5CTRL = PORT_PULLUPEN_bm;
	PORTC.PIN6CTRL = PORT_PULLUPEN_bm;
	PORTC.PIN7CTRL = PORT_PULLUPEN_bm;

	// Set up pins
	DATA_PORT_INPUT; 		// Start as input so high-Z
	disableAddressBusCtrl();
	disableControlSignals();

	serial.begin();

	/***************************************************************************
	 *****   MAIN LOOP													   *****
	 **************************************************************************/

	char cmdBuf[CMD_BUF_LEN];
	// uint8_t cmdBufIdx = 0;
	bool cmdRecvd = false;
	//bool msgRecvd = false;
	//bool dataRecvd = false;
	serial.writeln("STRT");

	while (1) {
		if (cmdRecvd) {
			// bool error = false;
			serial.clearInputBuffer();
			serial.write("ACKN");

			enableControlSignals();
			enableAddressBusCtrl();

			// -----------------------------------------------------------------
			// ----- BURN - Download data & write to Flash ---------------------
			// -----------------------------------------------------------------
			// Write the contents of the RAM to the Flash memory.
			if (strcmp(cmdBuf, "BURN") == 0) {
				dataSize = 0; // reset
				bool error = false;
				error = checkForMessage("SIZE", cmdBuf);
				if (!error) {
					// Send back 'SIZE'
					serial.write("SIZE");
					// Get the two bytes with the data sixe
					dataSize = getWord();
					// Send back two file size data bytes, MSB first
					sendWord(dataSize);

					error = checkForMessage("WFLS", cmdBuf);
					if (!error) {
						// Send the same message back as confirmation.
						serial.write("WFLS");
					} else {
						serial.write("*ERR");
					}
					// RECEIVE DATA
					if (!error) {
						uint16_t byteIdx = 0; // for writing to Flash
						uint16_t fileIdx = 0;
						uint8_t chunkIdx = 0;
						// uint8_t inByte = 0;
						DATA_PORT_OUTPUT;

						while (byteIdx < dataSize) {
							bool gotChunk = false;
							while (!gotChunk) {
								if (serial.inWaiting()) {
									dataChunk[chunkIdx] = serial.getByte();
									chunkIdx++;
									fileIdx++;
									if (chunkIdx == CHUNKSIZE || fileIdx == dataSize) {
										gotChunk = true;
									}
								}
							}
							// chunkIdx contains how many bytes we have.
							// It may be less than the chunksize if it's the
							// last chunk.

							// check if we're at a sector boundary
							// - clear sector
							if (byteIdx % FLASH_SECTOR_SIZE == 0) {
								sectorErase(byteIdx);
							}

							// write bytes
							for (uint8_t i = 0; i < chunkIdx; i++) {
								flashByteWrite(byteIdx, dataChunk[i]);
								byteIdx++;
							}
							chunkIdx = 0;
							if (byteIdx == dataSize) {
								serial.write("EODT"); // sent all bytes
							} else {
								serial.write("ACKN"); // to prompt sending of next chunk
							}
						}
						// CHECK DATA
						// Send back the first 16 bytes.
						// Read each one from RAM and send it across serial.
						DATA_PORT_INPUT;
						for (uint16_t addr = 0; addr < 16; addr++) {
							uint8_t testVal = readFlash(addr);
							serial.sendByte(testVal);
						}
					}
				}
			} else if (strcmp(cmdBuf, "CLRF") == 0) {
				// -------------------------------------------------------------
				// ----- CLRF - Clear flash                ---------------------
				// -------------------------------------------------------------
				// Clear the Flash memory.
				for (uint8_t sector = 0; sector < 4; sector++) {
					uint16_t addr = sector * FLASH_SECTOR_SIZE;
					sectorErase(addr);
					serial.write("SECT");
				}
				serial.write("DONE");
				// -------------------------------------------------------------
				// ----- READ - read Flash memory ------------------------------
				// -------------------------------------------------------------
				// Read 256 values from Flash memory, starting at a given
				// address.
			} else if (strcmp(cmdBuf, "READ") == 0) {
				// Confirm command received
				serial.write("READ");
				// Get address (two bytes) & relay it back.
				uint16_t address = getWord();
				sendWord(address);
				// Read Flash memory & send bytes
				for (uint16_t i = 0; i < 256; i++) {
					uint8_t byteVal = readFlash(address + i);
					serial.sendByte(byteVal);
				}
			} else {
				serial.write("*ERR");
			}
			// When done, reset
			clearBuf(cmdBuf, CMD_BUF_LEN);
			serial.clearInputBuffer();
			cmdRecvd = false;
			//cmdBufIdx = 0;

			disableControlSignals();
			disableAddressBusCtrl();
			DATA_PORT_INPUT;

		} else {
			getCommand(cmdBuf); // don't come back until you've got a message
			cmdRecvd = true;
		}

	}
}
