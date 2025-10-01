/*
	Macros for ATMEGA328PB
*/
#ifndef __MS_ATMEGA_4809P_H__
#define __MS_ATMEGA_4809P_H__

//namespace msavr {

#define PA0 0
#define PA1 1
#define PA2 2
#define PA3 3
#define PA4 4
#define PA5 5
#define PA6 6
#define PA7 7

#define PB0 0
#define PB1 1
#define PB2 2
#define PB3 3
#define PB4 4
#define PB5 5

#define PC0 0
#define PC1 1
#define PC2 2
#define PC3 3
#define PC4 4
#define PC5 5
#define PC6 6
#define PC7 7

#define PD0 0
#define PD1 1
#define PD2 2
#define PD3 3
#define PD4 4
#define PD5 5
#define PD6 6
#define PD7 7

#define PE0 0
#define PE1 1
#define PE2 2
#define PE3 3

#define PF0 0
#define PF1 1
#define PF2 2
#define PF3 3
#define PF4 4
#define PF5 5
#define PF6 6


// #define POB_REG 0x25 // Register addresses for ports, to be used in an
// #define POC_REG 0x28 // array of ports
// #define POD_REG 0x2B
// #define POE_REG 0x2E

// #define PIB_REG 0x23
// #define PIC_REG 0x26
// #define PID_REG 0x29
// #define PIE_REG 0x2C

#define U_CONTROLLER ATMEGA4809
// ----- SPI COMMS ----------------
// Compatible with 88/168/328P
// #define SPI_SS_GPIO         PB2
// #define SPI_SS_PORT         PORTB
// #define SPI_SS_PIN			PINB
// #define SPI_SS_DDR          DDRB

// #define SPI_MOSI_GPIO       PB3
// #define SPI_MOSI_PORT       PORTB
// #define SPI_MOSI_PIN	    PINB
// #define SPI_MOSI_DDR        DDRB

// #define SPI_MISO_GPIO       PB4
// #define SPI_MISO_PORT       PORTB
// #define SPI_MISO_PIN        PINB
// #define SPI_MISO_DDR        DDRB

// #define SPI_SCK_GPIO        PB5
// #define SPI_SCK_PORT        PORTB
// #define SPI_SCK_PIN         PINB
// #define SPI_SCK_DDR         DDRB

// ----- I2C COMMS ----------------
// Compatible with 88/168/328P
#define I2C_PORT	        PORTA
// #define I2C_PIN		        PINC
// #define I2C_DDR		        DDRC
#define I2C_SDA_GPIO        2
#define I2C_SCL_GPIO        3

// ----- SERIAL COMMS -------------
#define USART_TX_GPIO       0
#define USART_TX_PORT       PORTA
#define USART_RX_GPIO       1
#define USART_RX_PIN        PORTA

// Additional ports (not found on P version of chip)

// #define SPI_SS1_GPIO         PE2
// #define SPI_SS1_PORT         PORTE
// #define SPI_SS1_PIN			 PINE
// #define SPI_SS1_DDR          DDRE

// #define SPI_MOSI1_GPIO       PE3
// #define SPI_MOSI1_PORT       PORTE
// #define SPI_MOSI1_PIN	     PINE
// #define SPI_MOSI1_DDR        DDRE

// #define SPI_MISO1_GPIO       PC0
// #define SPI_MISO1_PORT       PORTC
// #define SPI_MISO1_PIN        PINC
// #define SPI_MISO1_DDR        DDRC

// #define SPI_SCK1_GPIO        PC1
// #define SPI_SCK1_PORT        PORTC
// #define SPI_SCK1_PIN         PINC
// #define SPI_SCK1_DDR         DDRC

// #define I2C1_PORT	         PORTE
// #define I2C1_PIN		     PINE
// #define I2C1_DDR		     DDRE
// #define I2C1_SDA_GPIO        PE0
// #define I2C1_SCL_GPIO        PE1

//};	// namespace msavr

#endif
