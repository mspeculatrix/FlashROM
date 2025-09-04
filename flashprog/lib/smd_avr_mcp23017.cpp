#include "smd_avr_mcp23017.h"

SMD_MCP23017::SMD_MCP23017(uint8_t address) : SMD_I2C_Device(address) {
	// _init(address, I2C_BUS_SPEED_STD, 0);
}

void SMD_MCP23017::begin() {
	// Set default configuration
	writeRegister(MCP23017_IOCON, MCP23017_IOCON_CFG);
	_delay_us(MCP23017_RW_DELAY);

	// Set ports to outputs by default
	writeRegister(MCP23017_IODIR + MCP23017_PORTA, 0x00);
	_delay_us(MCP23017_RW_DELAY);
	writeRegister(MCP23017_IODIR + MCP23017_PORTB, 0x00);
	_delay_us(MCP23017_RW_DELAY);

	// And set outputs to low
	writeRegister(MCP23017_GPIO + MCP23017_PORTA, 0x00);
	_delay_us(MCP23017_RW_DELAY);
	writeRegister(MCP23017_GPIO + MCP23017_PORTB, 0x00);
	_delay_us(MCP23017_RW_DELAY);
}

uint8_t SMD_MCP23017::readInput(uint8_t port) {
	return readRegister(MCP23017_GPIO + port);
}

// *** UNTESTED ***
// uint16_t SMD_MCP23017::readWord() {
// 	uint16_t value = (uint16_t)readInput(MCP23017_PORTA);
// 	value += (uint16_t)(readInput(MCP23017_PORTB << 8));
// 	return value;
// }

void SMD_MCP23017::setIODIR(uint8_t port, uint8_t config) {
	writeRegister(MCP23017_IODIR + port, config);
	_delay_us(MCP23017_RW_DELAY);
}

void SMD_MCP23017::setOutput(uint8_t port, uint8_t value) {
	writeRegister(MCP23017_GPIO + port, value);
	_delay_us(MCP23017_RW_DELAY);
}

// The following sets a 16-bit value (such as an address) on
// the two data ports, with the LSB on Port A and the MSB on
// Port B.
void SMD_MCP23017::setWord(uint16_t value) {
	setOutput(MCP23017_PORTA, (uint8_t)(value & 0x00FF));
	setOutput(MCP23017_PORTB, (uint8_t)(value >> 8));
}
