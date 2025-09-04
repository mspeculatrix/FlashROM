# User interface stuff, mostly Curses-related

import curses

LEFT_MARGIN: int = 5
MIN_COLS = 75
MIN_LINES = 20

MSGLINE: int = 1
INPUTLINE: int = MSGLINE + 1
STATUSLINE: int = INPUTLINE + 1
INFOLINE: int = STATUSLINE + 1
infoLineIdx: int = 0
MAXINFOLINES: int = 12
DATALINE: int = INFOLINE + MAXINFOLINES + 1
ERRLINE: int = DATALINE + 2


def anyKey(stdscr: curses.window) -> None:
	"""
	Wait for user to press any key before continuing.
	"""
	footerLine('Any key to continue', stdscr)
	_: int = stdscr.getch()


def clearInfo(stdscr: curses.window) -> None:
	"""
	Clear all the infolines in the display.
	"""
	global infoLineIdx
	for i in range(0, MAXINFOLINES):
		stdscr.move(INFOLINE + i, LEFT_MARGIN)
		stdscr.clrtoeol()
	stdscr.refresh()
	infoLineIdx = 0


def getHexInput(prompt: str, size: int, stdscr: curses.window) -> int:
	"""
	Get hex number input and return as an integer.
	"""
	inStr: str = ''
	charCount: int = 0
	xpos: int = LEFT_MARGIN + len(prompt) + 3
	stdscr.move(INPUTLINE, LEFT_MARGIN)
	stdscr.clrtoeol()
	stdscr.addstr(INPUTLINE, LEFT_MARGIN, prompt + ': ')
	stdscr.refresh()
	while charCount < size:
		newChar = stdscr.getkey().upper()
		if newChar in '0123456789ABCDEF':
			stdscr.addstr(INPUTLINE, xpos + charCount, newChar)
			stdscr.refresh()
			charCount += 1
			inStr += newChar
	return int(inStr, 16)


def getInput(prompt: str, line: int, stdscr: curses.window) -> str | None:
	"""
	Get user input and return as a string.
	"""
	inStr: str = ''
	xpos: int = LEFT_MARGIN + len(prompt) + 3

	stdscr.move(line, LEFT_MARGIN)
	stdscr.clrtoeol()
	stdscr.addstr(line, LEFT_MARGIN, prompt + ': ')
	stdscr.refresh()
	# input: bytes = stdscr.getstr(line, LEFT_MARGIN + len(prompt) + 3)
	done: bool = False
	charCount: int = 0
	while not done:
		newChar: int = stdscr.getch()
		stdscr.addstr(line, xpos + charCount, chr(newChar))
		stdscr.refresh()
		charCount += 1
		if newChar == 13 or newChar == 10:
			done = True
		else:
			inStr += chr(newChar)
	return inStr


def mainMenu(uploadState, romfile: str, stdscr: curses.window):
	"""
	Display the main menu and return the key selection.
	"""
	stdscr.clear()
	topLine('MAIN MENU', stdscr)
	printDataState(uploadState, stdscr)
	menuMsg = [
		'File: ' + romfile,
		'',
		'[F]ile selection',
		'[R]ead memory',
		'[U]pload data',
		'[W]rite data to Flash',
		'',
		'[Q]uit',
	]
	mline = STATUSLINE + 2
	key: str = ''
	for lineText in menuMsg:
		stdscr.addstr(mline, LEFT_MARGIN, lineText)
		mline += 1
	stdscr.refresh()
	gotKey = False
	while not gotKey:
		key = stdscr.getkey().upper()
		if key in ['Q', 'F', 'R', 'U', 'W']:
			gotKey = True
	return key


def printDataState(uploaded: bool, stdscr: curses.window) -> None:
	"""
	Show whether the currently selected file has been uploaded.
	"""
	msg: str = '-- not uploaded --'
	if uploaded:
		msg = 'UPLOADED'
	printline('DATA: ' + msg, STATUSLINE, stdscr)


def printInfoline(msg: str, stdscr: curses.window) -> None:
	"""
	Print a line of text in the infoline section of the screen.
	"""
	global infoLineIdx
	if infoLineIdx == MAXINFOLINES - 1:
		clearInfo(stdscr)
	stdscr.addstr(INFOLINE + infoLineIdx, LEFT_MARGIN, '- ' + msg)
	infoLineIdx += 1
	stdscr.refresh()


def printline(msg: str, lineNum: int, stdscr: curses.window) -> None:
	"""
	Print text on a specified line. Erases any older text first.
	"""
	stdscr.move(lineNum, LEFT_MARGIN)
	stdscr.clrtoeol()
	stdscr.addstr(lineNum, LEFT_MARGIN, msg)
	stdscr.refresh()


def footerLine(msg: str, stdscr: curses.window) -> None:
	"""
	Print text at the bottom of the screen. Mostly used for anyKey()
	"""
	btmStr: str = ' ' * LEFT_MARGIN + msg
	fillRpt: int = curses.COLS - len(btmStr) - 1
	btmStr += ' ' * fillRpt
	stdscr.addstr(curses.LINES - 1, 0, btmStr, curses.A_REVERSE)
	stdscr.refresh()


def topLine(msg: str, stdscr: curses.window) -> None:
	"""
	Print text at the top of the screen.
	"""
	topStr: str = ' ' * LEFT_MARGIN + msg
	fillRpt: int = curses.COLS - len(topStr) - 1
	topStr += ' ' * fillRpt
	stdscr.addstr(0, 0, topStr, curses.A_REVERSE)
	stdscr.refresh()
