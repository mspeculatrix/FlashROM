#include "smd_avr_mcp23008.h"

SMD_MCP23008::SMD_MCP23008(uint8_t address) : SMD_I2C_Device(address) {
	// _init(address, I2C_BUS_SPEED_STD, 0);
}

void SMD_MCP23008::begin() {
	writeRegister(MCP23008_IOCON, MCP23008_IOCON_CFG);
	_delay_us(MCP23008_RW_DELAY);
	writeRegister(MCP23008_IODIR, 0x00);	// Default to outputs
	_delay_us(MCP23008_RW_DELAY);
	writeRegister(MCP23008_GPIO, 0x00);		// Default to LOW
	_delay_us(MCP23008_RW_DELAY);
}

uint8_t SMD_MCP23008::readByte() {
	setIODIR(MCP23008_INPUT);
	return readInput();
}

uint8_t SMD_MCP23008::readInput() {
	uint8_t val = readRegister(MCP23008_GPIO);
	_delay_us(MCP23008_RW_DELAY);
	return val;
}

void SMD_MCP23008::setIODIR(uint8_t config) {
	writeRegister(MCP23008_IODIR, config);
	_delay_us(MCP23008_RW_DELAY);
}

void SMD_MCP23008::setOutput(uint8_t value) {
	writeRegister(MCP23008_GPIO, value);
	_delay_us(MCP23008_RW_DELAY);
}

void SMD_MCP23008::setByte(uint8_t dataByte) {
	setIODIR(MCP23008_OUTPUT);
	setOutput(dataByte);
}
