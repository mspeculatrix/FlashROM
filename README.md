# FlashROM

**This is a work in progress.**

This is a project to create a dev board for writing ROM images to flash memory.

I'm documenting the project on [Machina Speculatrix](https://medium.com/machina-speculatrix), my Medium-based publication. (A Medium sub is required to read the articles.)

I'm using the SST39SF010A flash chip, with a view to incorporating this into my [Zolatron 6502-based homebrew computer](https://medium.com/machina-speculatrix/subpage/0b8cf602629b) project.

The dev board consists of an ATMEGA328PB microcontroller, SRAM, port expanders to handle the data and address busses, the flash memory and a serial port to talk to a PC.

The code consists of:

- `flashprog`: C++ code for the ATMEGA328PB.
- `flashterm`: Python code to run on the PC. You could probably just use a serial terminal program.
