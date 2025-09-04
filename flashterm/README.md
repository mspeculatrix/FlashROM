# FlashTerm

This is designed to communicate with my flash programmer board running the `FlashProg` firmware.

It talks via serial with the microcontroller on the board.

All files to be uploaded to the Flash need to be in this program's `files` sub-directory.

## MAIN MENU

Menu options are:

- [F]ile selection. Choose a file from the `files` subdir. Defaults to `ROM.bin` on startup.
- [R]ead memory. Prompts for a 4-digit hex start address and then commands remote board to send the contents of 128 bytes of Flash memory starting at that adddress.
- [U]pload data. Uploads the contents of the currently selected file to the remote board's RAM.
- [W]rite data to Flash. Tells the remote board to go ahead and write the contents of RAM to the Flash memory.

## STATUS

Currently working and usable. Could be improved & expanded.

## TO DO

- Add command line options so that upload & flash routines can be called from a script.

## VERSION HISTORY

Dates indicate when `dev` branch was merged into `main`.

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
