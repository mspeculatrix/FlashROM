# FlashTerm

This is designed to communicate with my flash programmer board running the `FlashProg` firmware.

It talks via serial with the microcontroller on the board. I use an FTDI-based USB-to-serial cable to connect my Linux PC to the board. This comes up as `/dev/ttyUSB0` which I've hardcoded into the code.

All files to be uploaded to the Flash need to be in a `files` sub-directory below whatever directory you run this code from.

You need to have pyserial and curses installed.

## MAIN MENU

Menu options are:

- [F]ile selection. Choose a file from the `files` subdir. Defaults to `ROM.bin` on startup.
- [U]pload data. Uploads the contents of the currently selected file to the remote board's RAM.
- [W]rite data to Flash. Tells the remote board to go ahead and write the contents of RAM to the Flash memory.
- [R]ead flash memory. Prompts for a 4-digit hex start address and then commands remote board to send the contents of 256 bytes of Flash memory starting at that adddress.
- [S]how SRAM. Prompts for a 4-digit hex start address and then commands remote board to send the contents of 256 bytes of SRAM memory starting at that adddress.

## STATUS

Currently working and usable. Could be significantly improved & expanded.

## TO DO

- Add command line options so that upload & flash routines can be called from a script.

## VERSION HISTORY

### v1.3 - 24/09/2025

- Some bugs squashed.
- Really working now.

### v1.2 - 07/07/2025

- Added file selection. Working version.

### v1.1 - 06/07/2025

- Moved UI stuff to a package.
- Created file package.
- Added READ function.

### v1.0 - 04/07/2025

Everything basically working, including curses UI.

## Resources

- **Curses**: https://docs.python.org/3/library/curses.html
- **PySerial**: https://pyserial.readthedocs.io/en/latest/pyserial_api.html#classes
