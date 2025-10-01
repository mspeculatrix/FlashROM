# FLASHZ

NB: **WORK IN PROGRESS**

This is a version of the `flashprog` AVR code, but targeted to the ATmega4809 microcontroller.

## PIN ASSIGNMENTS

| Signal              | Port  | Pin    |
|--------------------:|:-----:|--------|
| Addr bus   A0 .. A5 | PORTC | 0 .. 5 |
|           A6 .. A11 | PORTF | 0 .. 5 |
|          A12 .. A15 | PORTE | 0 .. 3 |
| Data bus     D0..D7 | PORTD | 0 .. 7 |
| CPU_RDY             | PORTA | 2      |
| CPU_BE              | PORTA | 3      |
| SYS_PHI2            | PORTA | 4      |
| SYS_READ_EN / FL_OE | PORTA | 5      |
| SYS_ROM_EN  / FL_CE | PORTA | 6      |
| FL_WE               | PORTA | 7      |
| SYS_RES             | PORTF | 6      |
