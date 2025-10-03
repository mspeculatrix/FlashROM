# FLASHZ

This is a cut-down version of the `flashprog` AVR code, but targeted to the ATmega4809 microcontroller. It is also for a solution with no RAM, so writing of incoming ROM image data is done direct to the Flash memory. And it uses the MCU's GPIO pins to control the data and address lines, so no need for the I2C port expanders.

It is designed to be used with the `flashburn.py` Python program.

The code is based around the 40-pin DIP ('P') variant of the ATmega4809.

## ATmega4809-P PIN ASSIGNMENTS

| Signal              | Port  | Pin    |
|--------------------:|:-----:|--------|
| Addr bus   A0 .. A5 | PORTC | 0 .. 5 |
|           A6 .. A11 | PORTF | 0 .. 5 |
|          A12 .. A15 | PORTE | 0 .. 3 |
| Data bus     D0..D7 | PORTD | 0 .. 7 |
| CPU_RDY             | PORTA | 2      |
| CPU_BE              | PORTA | 3      |
| SYS_PHI2            | PORTA | 4      |
| SYS_ROM_EN  / FL_CE | PORTA | 5      |
| SYS_READ_EN / FL_OE | PORTA | 6      |
| FL_WE               | PORTA | 7      |
| SYS_RES             | PORTF | 6      |
