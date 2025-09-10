/*
 * Flash_ROM_programmer
 *
 * Works in conjunction with flashterm.py Python program.
 *
 * Created: 22/10/2023
 * Author : smd
 *
 */

#ifndef __AVR_ATmega328PB__
#define __AVR_ATmega328PB__
#endif

#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz unsigned long
#endif

#ifndef __OPTIMIZE__
#define __OPTIMIZE__
#endif

#define __DELAY_BACKWARD_COMPATIBLE__ // fix problem with _delay_ms()

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "lib/functions.h"
#include "lib/smd_std_macros.h"
#include "lib/smd_atmega_328PB.h"

#include "lib/smd_avr_seriallib.h"
#include "lib/smd_avr_i2clib.h"
#include "lib/smd_avr_mcp23008.h"
#include "lib/smd_avr_mcp23017.h"

#include "lib/flash_functions.h"
#include "lib/ram_functions.h"
#include "lib/defines.h"

using namespace smd_avr_i2c;

/*******************************************************************************
 *****   GLOBALS														   *****
 ******************************************************************************/

uint16_t addrPtr = 0;
uint16_t ramPtr = 0;
uint16_t dataSize = 0;

SMD_AVR_Serial serial = SMD_AVR_Serial(BAUDRATE);
SMD_MCP23008 dataport = SMD_MCP23008(0x20 << 1);
SMD_MCP23017 addrport = SMD_MCP23017(0x21 << 1);

/*******************************************************************************
 *****   FUNCTIONS														   *****
 ******************************************************************************/

 // PROTOTYPES
bool checkForMessage(const char* msg, char* buf);
uint8_t getCommand(char* buf);

// Look for a specific incoming message. Wrapper to getCommand()
bool checkForMessage(const char* msg, char* buf) {
	bool recvd = false;
	getCommand(buf);
	if (strcmp(buf, msg) == 0) {
		recvd = true;
	}
	return recvd;
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
	char wordBuf[2];
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
	/***************************************************************************
	 *****   SETUP			 											   *****
	 **************************************************************************/

	 // Set port DDRs - set all signals that need to start as outputs
	DDRB |= (1 << S1_LED | 1 << S2_LED);
	DDRC |= (1 << FL_OE | 1 << FL_WE | 1 << FL_CE | 1 << MCP_RESET);
	DDRD |= (1 << R_OE | 1 << R_WE | 1 << R_CE);
	DDRE |= (1 << A16); // set as output

	setLED(S1_LED, HIGH);
	setLED(S2_LED, HIGH);

	// Reset the port expanders.
	setPin(&PORTC, MCP_RESET, LOW);
	_delay_ms(10);
	setPin(&PORTC, MCP_RESET, HIGH);

	// Set all the Flash control pins high (ie, disable Flash)
	setPin(&PORTC, FL_OE, HIGH);
	setPin(&PORTC, FL_WE, HIGH);
	setPin(&PORTC, FL_CE, HIGH);

	// Set all the RAM control pins high (ie, disable RAM)
	setPin(&PORTD, R_OE, HIGH);
	setPin(&PORTD, R_WE, HIGH);
	setPin(&PORTD, R_CE, HIGH);

	// Set A16 pin LOW - not really using it yet.
	setPin(&PORTE, A16, LOW);


	serial.begin();

	smd_avr_i2c::I2C_enable_pullups(&I2C_PORT, I2C_SDA_GPIO, I2C_SCL_GPIO);
	dataport.begin();
	dataport.setIODIR(OUTPUT);

	addrport.begin();
	addrport.setIODIR(MCP23017_PORTA, OUTPUT);
	addrport.setIODIR(MCP23017_PORTB, OUTPUT);

	serial.writeln("Up and running");

	setLED(S1_LED, LOW);
	setLED(S2_LED, LOW);


	/***************************************************************************
	 *****   MAIN LOOP													   *****
	 **************************************************************************/
	char cmdBuf[CMD_BUF_LEN];
	uint8_t cmdBufIdx = 0;
	bool cmdRecvd = false;
	bool msgRecvd = false;
	bool dataRecvd = false;

	while (1) {
		if (cmdRecvd) {
			bool error = false;
			setLED(ACT_LED, ON);
			serial.clearInputBuffer();
			// -----------------------------------------------------------------
			// ----- DNLD - file download --------------------------------------
			// -----------------------------------------------------------------
			if (strcmp(cmdBuf, "DNLD") == 0) {
				// We send 'ACKN' after which the remote host should send
				// 'SIZE', if it received the ACKN okay.
				// If it didn't, it will resend DNLD
				dataSize = 0; // reset
				bool handShake_ok = false;
				uint8_t attempts = 0;
				while (!handShake_ok && !error) {
					attempts++;
					serial.write("ACKN");
					handShake_ok = checkForMessage("SIZE", cmdBuf);
					if (!handShake_ok && attempts == MAX_MSG_TRIES) {
						error = true;
					}
				}
				// We have received 'SIZE'. Now get the next two bytes which
				// tell us the file size.
				if (!error) {
					dataSize = getWord();
					// Send back 'SIZE' and the two size bytes to confirm we
					// got the data okay.
					serial.write("SIZE");
					// Send two file size data bytes, MSB first
					sendWord(dataSize);
					// If the remote determines that the sizes match, it sends
					// 'WMEM'
					if (checkForMessage("WMEM", cmdBuf)) {
						// Send the same message back as confirmation.
						serial.write("WMEM");
					} else {
						serial.write("*ERR");
						error = true;
					}
					// RECEIVE DATA
					if (!error) {
						setLED(ACT_LED, HIGH);
						uint16_t dataIdx = 0; // for writing to RAM
						uint8_t inByte = 0;
						dataport.setIODIR(OUTPUT); 	// Set data port to output
						while (dataIdx < dataSize) {
							while (serial.readByte(&inByte)) {
								writeRAM(dataIdx, inByte);
								// serial.sendByte(inByte);
								_delay_us(1);
								dataIdx++;
							}
						}
						setLED(ACT_LED, LOW);
					}
					serial.write("DONE");

					// CHECK DATA
					// Start by sending back the first 16 bytes.
					// Read each one from RAM and send it across serial.
					dataport.setIODIR(INPUT);
					for (uint16_t addr = 0; addr < 16; addr++) {
						uint8_t testVal = readRAM(addr);
						serial.sendByte(testVal);
					}
					// Look for 'CONF' message from remote.
					if (checkForMessage("CONF", cmdBuf)) {
						dataRecvd = true;
					} else {
						dataRecvd = false; // just to be sure
					}

				} // if(!error)

			// -----------------------------------------------------------------
			// ----- FLSH - Flash ----------------------------------------------
			// -----------------------------------------------------------------
			// Write the contents of the RAM to the Flash memory.
			} else if (strcmp(cmdBuf, "FLSH") == 0) {
				// Confirm receipt of command.
				serial.write("FLSH");
				if (dataSize > 0) {
					uint16_t addrIdx = 0;
					while (addrIdx < dataSize) {
						// When at the beginning of each sector of the Flash
						// memory, we need to erase the sector.
						if (addrIdx % FLASH_SECTOR_SIZE == 0) {
							sectorErase(addrIdx);
						}
						// Read next byte from RAM
						uint8_t byteVal = readRAM(addrIdx);
						// and write it to Flash
						flashByteWrite(addrIdx, byteVal);
						addrIdx++;
					}
					// now send confirmation bytes
					serial.write("TSTD");
					// Read 16 bytes from Flash memory and send over serial
					for (uint8_t i = 0; i < 16; i++) {
						uint8_t byteVal = readFlash(i);
						serial.sendByte(byteVal);
					}
					serial.write("DONE");
				} else {
					serial.write("*ERR");
				}
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
			setLED(ACT_LED, OFF);
			clearBuf(cmdBuf, CMD_BUF_LEN);
			serial.clearInputBuffer();
			cmdRecvd = false;
			cmdBufIdx = 0;
		} else {
			getCommand(cmdBuf); // don't come back until you've got a message
			cmdRecvd = true;
		}

	}

}
