#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__


#include <avr/io.h>

void clearBuf(char* buf, uint8_t len);
void setLED(uint8_t led, uint8_t state);
void setPin(volatile uint8_t* portreg, uint8_t pin, uint8_t hilo);

void clearBuf(char* buf, uint8_t len) {
	for (uint8_t i = 0; i < len; i++) {
		buf[i] = 0;
	}
}

void setLED(uint8_t led, uint8_t state) {
	setPin(&PORTB, led, state);
}

void setPin(volatile uint8_t* portreg, uint8_t pin, uint8_t hilo) {
	if (hilo == 1) {
		*portreg |= (1 << pin);
	} else {
		*portreg &= ~(1 << pin);
	}
}

#endif
