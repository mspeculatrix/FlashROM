# FLASHTERM.PY

Python program designed to communicate with my flash programmer board running the `flashprog` firmware.

It talks via serial with the microcontroller on the board. I use an FTDI-based USB-to-serial cable to connect my Linux PC to the board. This comes up as `/dev/ttyUSB0` which I've hardcoded into this program.

All files to be uploaded to the Flash need to be in a `files` sub-directory below whatever directory you run this code from.

Currently, the main program imports two modules in the `funcs` directory. For greater portability, I may incorporate those functions into the main code at some time.

You need to have pyserial and curses installed.

## MAIN MENU

Menu options are:

- `[F]ile selection` : Choose a file from the `files` subdir. Defaults to `ROM.bin` on startup.
- `[C]lear Flash` : Run a sector erase on all sectors.
- `[U]pload data to RAM` : Uploads the contents of the currently selected file to the remote board's RAM.
- `[W]rite RAM data to Flash` : Tells the remote board to go ahead and write the contents of RAM to the Flash memory.
- `[B]urn - upload & write direct to Flash` : Does what it says, ignoring RAM.
- `[R]ead Flash memory` : Prompts for a 4-digit hex start address and then commands remote board to send the contents of 256 bytes of Flash memory starting at that adddress.
- `[S]how RAM memory` : Prompts for a 4-digit hex start address and then commands remote board to send the contents of 256 bytes of SRAM memory starting at that adddress.

## STATUS

Currently working and usable. Could be improved & expanded. But it's at a stage where my main focus is elsewhere.

## Resources

- **Curses**: https://docs.python.org/3/library/curses.html
- **PySerial**: https://pyserial.readthedocs.io/en/latest/pyserial_api.html#classes
