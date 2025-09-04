# Flash ROM programmer

AVR Atmega328p code to program SST39SF010 flash chips. This is for experimenting with using flash chips in place of eeproms.

All operations are initiated by the remote computer which is running my `flashterm` Python program.

The code waits in a loop for a command coming down the serial connection. These commands can be:

- DNLD - to download binary data over the serial connection and store it in RAM.
- FLSH - to write the code in RAM to the flash chip.
- READ - read 128 bytes of Flash memory starting at the given address and send it over serial.

## DOWNLOAD (DNLD)

Receives data from remote machine and stores it in RAM. Sends back first 16 bytes of RAM to the remote program to confirm receipt.

## FLASH (FLSH)

Writes the contents of RAM to the Flash chip. Sends back the first 16 bytes of Flash memory as confirmation,

## READ (READ)

Read 128 bytes of Flash memory starting at the given address and send them over serial. Doesn't currently do any checking that we're not exceeding the top of memory, and it probably should do this.

## VERSION HISTORY

### v1.2 - 07/07/2025

TO DO:

- When writing to/reading from RAM and Flash, check that we don't exceed the memory limits.
- Writing code to various banks. A large flash chip could effectively offer multiple ROMs in one chip.

### v1.1 - 06/07/2025

- Moved RAM functions to separate header file.
- Added READ function.

### v1.0 - 04/07/2025

All basic functions complete & working (as far as I can tell).
