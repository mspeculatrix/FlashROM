#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__


#include <avr/io.h>

void clearBuf(char* buf, uint8_t len) {
	for (uint8_t i = 0; i < len; i++) {
		buf[i] = 0;
	}
}

void setPin(volatile uint8_t* portreg, uint8_t pin, uint8_t hilo) {
	if (hilo == 1) {
		*portreg |= (1 << pin);
	} else {
		*portreg &= ~(1 << pin);
	}
}

#endif
