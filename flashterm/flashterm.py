#!/usr/bin/env python3

import curses
import time

import funcs.file
import funcs.ui
import serial

MAX_HANDSHAKES: int = 4
VERSION: str = '1.3'
SERIAL_PORT: str = '/dev/ttyUSB0'  # configure for your machine
BAUDRATE: int = 9600  # fast enough


stdscr: curses.window = curses.initscr()

# Check window is large enough.
if curses.LINES < funcs.ui.MIN_LINES or curses.COLS < funcs.ui.MIN_COLS:
	print('***ERROR***')
	print(f'Screen must be at least {funcs.ui.MIN_LINES} lines high')
	print(f'and {funcs.ui.MIN_COLS} columns wide')
	exit()

curses.curs_set(False)  # turn off the flashing cursor

ser = serial.Serial(
	SERIAL_PORT,
	BAUDRATE,
	bytesize=serial.EIGHTBITS,
	parity=serial.PARITY_EVEN,
	stopbits=serial.STOPBITS_ONE,
	timeout=1,
)


def clearSerial() -> None:
	"""
	Empty out the serial buffers, just to be sure.
	"""
	ser.reset_input_buffer()
	ser.reset_output_buffer()
	while ser.in_waiting > 0:
		_ = ser.read(1)


def getTestData(
	numBytes: int, checkBuf: list[bytes] | None = None
) -> tuple[list[bytes], bool]:
	"""
	Get numBytes bytes from the serial port. If desired, check these against a
	buffer (a list of bytes) for any discrepancies.
	"""
	mismatch = False
	dataItems: list[bytes] = []
	for i in range(0, numBytes):
		testByte = ser.read(1)  # blocks until byte is read
		if checkBuf is not None and checkBuf[i] != testByte:
			mismatch = True
		dataItems.append(testByte)
	return dataItems, mismatch


def hexStr(value, length=2) -> str:
	"""
	Format a numeric value as a hex string.
	"""
	fmt = '{0:0' + str(length) + 'X}'
	return fmt.format(value)


def printBuf(buffer: list[bytes], num: int, line: int) -> None:
	"""
	Take a list of bytes and print as a string of space-separated hex values.
	"""
	items = []
	for i in range(0, num):
		items.append(hexStr(buffer[i][0]))
	funcs.ui.printline(' '.join(items), line, stdscr)


def readMemory(cmd: str) -> None:
	"""
	Prompt for a memory start address, send that to the remote device and
	get back n bytes of data.
	"""
	clearSerial()
	stdscr.clear()
	funcs.ui.topLine('READ MEMORY', stdscr)
	addr: int = funcs.ui.getHexInput('Address', 4, stdscr)
	funcs.ui.printline(str(addr), funcs.ui.STATUSLINE, stdscr)
	stdscr.clear()
	funcs.ui.topLine('READ MEMORY', stdscr)
	funcs.ui.clearInfo(stdscr)
	funcs.ui.printline(
		f'Start address: 0x{addr:04X} {addr}', funcs.ui.INPUTLINE, stdscr
	)
	if cmd == 'SRAM' and addr > 0x7FFF:
		funcs.ui.printError('Memory address too large for RAM', stdscr)
	else:
		# send command
		writeCmd: bytes = cmd.encode('ascii')
		ser.write(writeCmd)
		ser.write(b'\n')
		response: bool = False
		while not response:
			msg_in = ser.read(4)
			if msg_in == writeCmd:
				funcs.ui.printline('command received', funcs.ui.STATUSLINE, stdscr)
				response = True
				# Send address and read response
				sendWord(addr)
				addrResp = readWord()
				if addr == addrResp:
					funcs.ui.printline(
						f'Addresses match: {addr:04X} {addrResp:04X}',
						funcs.ui.STATUSLINE,
						stdscr,
					)
					# get 256 bytes back, 16 at a time
					for i in range(0, 16):
						stdscr.refresh()
						dataItems, _ = getTestData(16)
						printBuf(dataItems, 16, funcs.ui.INFOLINE + 2 + i)
				else:
					funcs.ui.printError(
						f'Addresses do not match {addr:04X} {addrResp:04X}',
						stdscr,
					)
	clearSerial()
	funcs.ui.anyKey(stdscr)


def readWord() -> int:
	"""
	Get 2 bytes from the serial port and convert to a 16-bit integer. Expects
	byte order to be MSB-first.
	"""
	value: int = 0
	hi: bytes = ser.read(1)
	lo: bytes = ser.read(1)
	# funcs.ui.printline(f'{hi} {lo}', funcs.ui.DATALINE, stdscr)
	if hi and lo:
		value = (ord(hi) << 8) + ord(lo)
	return value


def sendWord(word: int):
	"""
	Take a 16-bit value and send it across serial as 2 bytes, MSB-first.
	"""
	hi_byte: bytes = chr(word >> 8).encode('latin-1')
	lo_byte: bytes = chr(word & 0x00FF).encode('latin-1')
	# funcs.ui.printline(f'Word: {hi_byte} {lo_byte}', 35, stdscr)
	ser.write(hi_byte)
	ser.write(lo_byte)


def uploadData(datafile: str) -> bool:
	"""
	Send the contents of the file buffer (a list of bytes) to the board
	for storage in RAM.
	"""
	clearSerial()
	stdscr.clear()
	funcs.ui.topLine('UPLOADING', stdscr)
	funcs.ui.clearInfo(stdscr)
	funcs.ui.printline('File: ' + datafile, funcs.ui.MSGLINE, stdscr)

	clearSerial()
	data_uploaded = False
	fault = False
	fileBuf: list[bytes] = []

	# file_size, _ = readFile(fileBuf, romfile)
	fileBuf, file_size, error = funcs.file.readFile(datafile)
	if error is None:
		funcs.ui.printInfoline(f'filesize: 0x{hexStr(file_size, 2)}', stdscr)
		printBuf(fileBuf, 16, funcs.ui.DATALINE)
	else:
		fault = True

	# STEP 1: Handshake
	handshake_ok: bool = False
	handshake_attempts: int = 0
	funcs.ui.printline('Starting handshake...', funcs.ui.MSGLINE, stdscr)
	while not handshake_ok and not fault:
		clearSerial()
		handshake_attempts += 1
		funcs.ui.printInfoline('sending DNLD', stdscr)
		ser.write(b'DNLD\n')
		msg_in = ser.read(4)
		if msg_in == b'ACKN':
			handshake_ok = True
			funcs.ui.printInfoline('received ACKN', stdscr)
			funcs.ui.printInfoline('handshake complete', stdscr)
		elif handshake_attempts == MAX_HANDSHAKES:
			funcs.ui.printError('failed to complete handshake', stdscr)
			fault = True
		else:
			funcs.ui.printError(f'got: {msg_in}', stdscr)

	# STEP 2: Transmit & agree on file size
	if not fault:
		funcs.ui.printline('Sending SIZE', funcs.ui.MSGLINE, stdscr)
		ser.write(b'SIZE\n')
		sendWord(file_size)
		msg_in = ser.read(4)
		if msg_in == b'SIZE':
			funcs.ui.printInfoline('received: SIZE', stdscr)
			rec_file_sz = readWord()
			funcs.ui.printInfoline(
				f'received filesize: 0x{hexStr(rec_file_sz, 4)}', stdscr
			)
			if file_size == rec_file_sz:
				funcs.ui.printInfoline('sizes match!', stdscr)
			else:
				funcs.ui.printError('size mismatch', stdscr)
				fault = True
		else:
			fault = True

	# STEP 3: Send Write Memory command
	if not fault:
		clearSerial()
		funcs.ui.printline('Sending WMEM', funcs.ui.MSGLINE, stdscr)
		ser.write(b'WMEM\n')
		response_ok = False
		attempts: int = 0
		while not response_ok and not fault:
			attempts += 1
			msg_in = ser.read(4)
			if msg_in == b'WMEM':
				funcs.ui.printInfoline('received: WMEM', stdscr)
				response_ok = True
			elif attempts == MAX_HANDSHAKES:
				funcs.ui.printline(f'Got: {msg_in}', funcs.ui.STATUSLINE, stdscr)
				fault = True
			else:
				funcs.ui.printline(f'Got: {msg_in}', funcs.ui.STATUSLINE, stdscr)
				time.sleep(0.01)  # 10ms

	# STEP 4 - send data
	if not fault:
		funcs.ui.printline('Sending data...', funcs.ui.MSGLINE, stdscr)
		for i in range(0, len(fileBuf)):
			funcs.ui.printline(f'{hexStr(i)}', funcs.ui.STATUSLINE, stdscr)
			ser.write(fileBuf[i])
			# print(hexStr(ser.read(1)), end=' ', flush=True)
		funcs.ui.printline(f'{hexStr(len(fileBuf))}', funcs.ui.STATUSLINE, stdscr)

		# STEP 5 - check response
		response_ok = False
		while not response_ok and not fault:
			msg_in: bytes = ser.read(4)
			if msg_in == b'DONE':
				funcs.ui.printInfoline('received: DONE', stdscr)
				response_ok = True
				mismatch: bool = False
				# Expect 16 bytes back from client containing test data
				dataItems, mismatch = getTestData(16, fileBuf)
				if mismatch:
					funcs.ui.printError('data test mismatch', stdscr)
					ser.write(b'*ERR\n')
				else:
					funcs.ui.printInfoline('data check OK', stdscr)
					printBuf(dataItems, 16, funcs.ui.DATALINE + 1)
					data_uploaded = True
					ser.write(b'CONF\n')
	funcs.ui.anyKey(stdscr)
	return data_uploaded


def writeData() -> bool | None:
	"""
	Tell the remote board to go ahead and write the data it has in RAM
	to the Flash.
	"""
	error = None
	global infoLineIdx
	infoLineIdx = 0
	stdscr.clear()
	funcs.ui.topLine('WRITE DATA', stdscr)
	funcs.ui.clearInfo(stdscr)
	funcs.ui.printline(
		'Writing uploaded data to flash memory', funcs.ui.MSGLINE, stdscr
	)
	funcs.ui.printInfoline('sending FLSH command', stdscr)
	ser.write(b'FLSH\n')
	response: bool = False
	complete: bool = False
	while not response or not complete:
		msg_in = ser.read(4)
		if msg_in == b'FLSH':
			funcs.ui.printInfoline('command received', stdscr)
			funcs.ui.printInfoline('waiting for write to complete...', stdscr)
			response = True
		elif msg_in == b'ADDR':
			addr: int = readWord()
			info: str = f'    Sector: 0x{addr:04X}'
			funcs.ui.printInfoline(info, stdscr)
		elif msg_in == b'TSTD':
			funcs.ui.printInfoline('receiving test data', stdscr)
			testData, _ = getTestData(16)
			printBuf(testData, 16, funcs.ui.DATALINE)
		elif msg_in == b'DONE':
			funcs.ui.printInfoline('completed', stdscr)
			complete = True
		# else:  # anything else is an error
		# 	funcs.ui.printInfoline(msg_in.decode('ascii'))
		# 	funcs.ui.printError('Unknown command', stdscr)
		# 	error = True
		# 	break
	funcs.ui.anyKey(stdscr)
	return error


# ------------------------------------------------------------------------------#
# ----  MAIN                                                                ----#
# ------------------------------------------------------------------------------#


def main(stdscr) -> None:
	dataUploaded: bool = False
	romfile: str = 'ROM.bin'

	############################################################################
	#####   MAIN LOOP                                                      #####
	############################################################################
	loop = True
	while loop:
		key = funcs.ui.mainMenu(dataUploaded, SERIAL_PORT, BAUDRATE, romfile, stdscr)
		if key == 'Q':  # Quit
			loop = False
		elif key == 'F':  # Select the data file
			funcs.ui.clearInfo(stdscr)
			romfile = funcs.file.setFile(romfile, stdscr)
		elif key == 'R':  # Read a block of data from Flash memory
			readMemory('READ')
		elif key == 'S':  # Read a block of data from RAM
			readMemory('SRAM')
		elif key == 'U':  # Upload data to RAM
			dataUploaded = uploadData(romfile)
		elif key == 'W':  # Write RAM data to Flash
			if dataUploaded:
				writeData()
			else:
				funcs.ui.printError('No data uploaded', stdscr)
				funcs.ui.anyKey(stdscr)
	ser.close()


if __name__ == '__main__':
	curses.wrapper(main)
	stdscr.clear()
