# FLASHBURN.PY

NB: **WORK IN PROGRESS**

Python code.

A simplified version of `flashterm` for use with dev scripts. There's no curses or UI. It simply takes a binary file and loads it to the Flash without involving RAM.

This is designed to work with the ATmega4809 version of the MCU code - `flashz`. It doesn't wotj with `flashprog` because of reasons.

## Command line options

- `-f <filename>` : Filename. Default is: `ROM.bin`.
- `-q` : Quiet. Verbose mode off.
- `-v` : Verbose. Prints messages to the screen.
