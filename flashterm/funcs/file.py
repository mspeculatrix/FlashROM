# Handy file-related functions.

import curses
import os
from pathlib import Path

from . import ui


def readFile(filename: str) -> tuple[list[bytes], int, str | None]:
	"""
	Read the contents of a binary file into a list of bytes and return that,
	along with the number of bytes read.
	"""
	fpath: Path = Path('files') / filename
	buffer: list[bytes] = []
	size = 0
	error: str | None = None
	if os.path.isfile(fpath):
		try:
			with open(fpath, 'rb') as fh:
				while True:
					fileByte: bytes = fh.read(1)
					if not fileByte:
						break
					buffer.append(fileByte)
					size += 1
		except IOError:
			# print('Cannot open file:', filepath, ' : ', e)
			error = 'Cannot open file:' + filename
	return buffer, size, error


def setFile(currPath: str, stdscr: curses.window) -> str:
	"""
	Select a file. Shows the files in the `files` subdir & prompts user to
	input a name. Checks that the given name is actually a file in the `files`
	subdir.
	"""
	filename: str = currPath
	stdscr.clear()
	ui.topLine('SELECT FILE', stdscr)
	ui.printline('Current path: ' + currPath, ui.MSGLINE, stdscr)

	files = []
	for _, _, filenames in os.walk('files'):
		files.extend(filenames)
		break  # break first time it yields to get only one dir deep

	counter: int = 0
	ypos: int = ui.INFOLINE
	for file in files:
		stdscr.addstr(ypos, ui.LEFT_MARGIN + counter * 18, file)
		counter += 1
		if counter % 4 == 0:
			counter = 0
			ypos += 1
	ypos += 2
	newfile: str | None = ui.getInput('file', ypos, stdscr)
	if newfile:
		if os.path.isfile(Path('files') / newfile):
			filename = newfile
		else:
			ui.printline('**ERR: file not found', ui.ERRLINE, stdscr)
			stdscr.refresh()
			ui.anyKey(stdscr)
	return filename

	ui.anyKey(stdscr)
