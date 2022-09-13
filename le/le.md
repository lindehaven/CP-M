# Lean Editor 

_A small text editor for programmers_


## Summary 

Lean Editor (LE) enables easy editing of small source code files.
Source code files are limited to 2000 lines with lengths less than
80 characters. The reasons are to simplify the design of LE and
that source code files should be small.

___NOTE!___
  If ^S is pressed repeatedly then screen output may pause.
  Press ^Q once to unpause screen output.
  Press ^Q again to refresh the screen.


## Screen Layout 

The first row on top of the screen is called the status row.
It displays:

* F: filename
* P: number of lines in the paste buffer
* E: number of edits that have been made
* T: tab width
* R: current row / last row
* C: current column / last column

_Example_
    `F:LE.C                       P:04  E:00056  T:4  R:0094/1000  C:09/68`

If an incremental string search is active then it is shown instead
of the file name:

*   S   incremental search string

_Example_
    `S:for (i =                   P:00  E:00119  T:4  R:0382/1000  C:21/52`


## Navigation Keys 

    ^E      Cursor one row up.
    ^X      Cursor one row down.
    ^S      Cursor one column left.
    ^D      Cursor one column right.
    ^A      Cursor one word left.
    ^F      Cursor one word right.
    ^J      Cursor to beginning of line.
    ^K      Cursor to end of line.
    ^R      Cursor one page up.
    ^C      Cursor one page down.
    ^T      Cursor to beginning of file.
    ^V      Cursor to end of file.
    ^Z      Center current row.
    ^Q      Unpause screen output and update screen.
    ^L      Search string incrementally. Looks for search string in the
            edit buffer and marks it with inverse video. Wraps when
            reached the end of the edit buffer.

## Editing Keys 

    ^U      Change tab width. Can be 1-8 spaces.
    ^I      Insert tab (Tab).
    ^M      Insert new line (Enter).
    ^G      Delete character to the right (Delete).
    ^H      Delete character to the left (Backspace).
    ^O      Cut complete line. Move line to paste buffer.
    ^P      Paste. Insert line(s) from paste buffer.
    ^Y      Move line up.
    ^B      Move line down.


## File Command Keys 

    ^W      Write to file. Saves the edit buffer to file.
    ESC     Escape. Exits the program if no changes have been made or the
            user does not want to save changes.


## Internals 

* Digital Research CP/M systems
* ANSI terminal
* Aztec C Compiler Vers. 1.06D by Manx Software Systems
* YAZE-AG 2.51.1 by Frank D. Cringle, Michael Haardt and Andreas Gerlich


## License 
_
    Copyright (C) 2017-2022 Lars Lindehaven.

    Work based on the Program Text editor (te) v1.08 from 05 Jul 2017.
    Copyright (c) 2015-2016 Miguel Garcia / FloppySoftware.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the free Software Foundation; either version 2, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for m