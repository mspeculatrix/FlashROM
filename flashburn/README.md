# FLASHBURN.PY

NB: **WORK IN PROGRESS**

Python code.

A simplified version of `flashterm` for use with dev scripts. There's no curses or UI. It simply takes a binary file and loads it to the Flash without involving RAM.

Also allows you to read the contents of the flash and (when I've located the bug) erase the flash memory.

This is designed to work with the ATmega4809 version of the MCU code - `flashz`. It doesn't wotj with `flashprog` because of reasons.

## Command line options

- `-e` : Erase Flash.
- `-f <filename>` : Use this file. Default is: `ROM.bin`.
- `-q` : Quiet. Verbose mode off.
- `-r <hhhh>` : Read contents of flash starting at hex address given. Must be a 4-digit address.
- `-v` : Verbose. Prints messages to the screen.

## CURRENT ISSUES

- The flash erase option seems not to work.
- The read option produces some interesting bugs when used twice in a row.

## TO DO

- Allow for writing to & reading from multiple banks. Expand writing, erasing & reading options to 17-bit or even 18-bit.
