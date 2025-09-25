# Flash ROM programmer

AVR Atmega328p code to program SST39SF010 flash chips. This is for experimenting with using flash chips in place of eeproms.

All operations are initiated by the remote computer which is running my `flashterm` Python program.

The code waits in a loop for a command coming down the serial connection. These commands can be:

- DNLD - to download binary data over the serial connection and store it in RAM.
- FLSH - to write the code in RAM to the flash chip.
- READ - read 128 bytes of Flash memory starting at the given address and send it over serial.
- SRAM - read 128 bytes of RAM starting at the given address and send it over serial.

## DOWNLOAD (DNLD)

Receives data from remote machine and stores it in RAM. There's no flow control - it all comes over in a single rush. Sends back first 16 bytes of RAM to the remote program to confirm receipt.

## FLASH (FLSH)

Writes the contents of RAM to the Flash chip. Sends back the first 16 bytes of Flash memory as confirmation.

## READ (READ)

Reads 256 bytes of Flash memory starting at the given address and send sthem over serial. Doesn't currently do any checking that we're not exceeding the top of memory, and it probably should do this.

## SHOW RAM (SRAM)

Reads 256 bytes of RAM starting at the given address and sends them over serial. Doesn't currently do any checking that we're not exceeding the top of memory, and it probably should do this.

## TO DO

- When writing to/reading from RAM and Flash, check that we don't exceed the memory limits.
- Writing code to various banks. A large flash chip could effectively offer multiple ROMs in one chip.
