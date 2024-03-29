# Micro Editor (ue) 


## Summary 

Micro Editor (UE) enables easy editing of small source code files. Source
code files are limited in size to fit into the transient program area
(TPA) of CP/M.

___NOTE!___
    If ^S is pressed repeatedly then screen output may pause. 
    Press ^Q once to unpause screen output.
    Press ^Q again to refresh the screen.


## Command Line Options 

UE can be started with a number of optional attribute values.

  `UE.COM [filename [tabwidth [autoindent [rows [columns]]]]]`

Default values for optional attributes are:
    
* filename    = $$$$$$$$.$$$
* tabwidth    = 8
* autoindent  = 1 (on)
* rows        = 25
* columns     = 80

_Examples_
  `UE.COM UE.C 4 0`
  `UE.COM NARROW.ASM 2 1 25 40`


## Files 

When UE reads a file it discards all control characters except TAB (0x09)
and LF (0x0a). When UE saves a file it adds a CR (0x0d) to each LF so that
end of line is according to CP/M standard. Lines can be of arbitrary length
but should, for the sake of reading and editing source code, be shortened
to fit inside terminal screen borders.


## Undo 

All changes are placed in the undo buffer and can be reverted by pressing
^Z (Undo last insert/delete). In case the undo buffer becomes full it needs
to be emptied by pressing ^Z (Undo last insert) or ^W (Write to file).


## Search & Replace 

A string can be searched incrementally by pressing ^L (Search string
incrementally) and then typing the string to search for. If the search
string is found then the cursor will move to the string in the edit buffer
and display it in inverse video. If the search string is not found then the
cursor will move to the beginning of the file. Pressing ^L again will
search for the next occurrence of the search string.

A string can be replaced by pressing ^N (Replace string) and then typing
the new string. If the search string has been found then it will be
possible to replace it with the new string. Pressing ^N again will replace
the search string with the new string and search for the next occurrence of
the search string. Pressing ^L again will search for the next occurrence of
the search string without replacing it.

Pressing ^[ (Escape), ^E (Cursor one row up), ^X (Cursor one row down),
^S (Cursor one column left) or ^D (Cursor one column right) exits the
search/replace mode and returns to edit mode.


## Syntax Highlighting 

To give more help for programmers, UE can be linked with the Syntax
Highlighter (SHL) module to highlight source code while editing.

This feature has been tested in the YAZE-AG simulator running on different
hardware platforms and operating systems. Using UE with SHL on a hardware
platform with a 4MHz CPU is not recommended (and not usable).

In order to reduce the CPU load, the highlighting of multi-line comments is
not continuously updated while typing. However, by pressing ^Q the edit
buffer is parsed completely and the source code is highlighted on the screen.


## Screen layout 

In case lines are too long to be displayed in full then the rightmost
column is marked with a '+' to indicate that there are more characters
beyond the right screen border.

The last row of the screen is called the status row and displays:

* Filename: filename
* U: undo buffer usage
* T: tabwidth
* C: current column
* R: current row / last row

_Example_
  `Filename: $$$$$$$$.$$$                 U:000% T:8 C:009 R:00094/00463`

If an incremental string search is active then it is shown instead of the
filename:
    
* Search for: <search string to look for>

_Example_
  `Search for: while (*p++                U:009% T:4 C:021 R:00382/00463`

If an incremental string replacements is active then it is shown instead
of the file name:
    
* Replace with: <new string to replace search string>

_Example_
  `Replace with: while (*ptr++            U:009% T:4 C:021 R:00382/00463`


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
    ^Q      Unpause screen output and update screen.
    ^L      Search string incrementally. Looks for search string in the
            edit buffer and marks it with inverse video. Wraps when
            reached the end of the edit buffer.


## Editing Keys 

    ^N      Replace string. Replaces a search string with a new string.
    ^U      Change tabwidth. Can be 2-8 spaces.
    ^I      Insert tab (Tab).
    ^M      Insert new line (Enter).
    ^G      Delete character to the right (Delete).
    ^H      Delete character to the left (Backspace).
    ^Z      Undo last insert/delete.
    ^O      Cut to end of line. Moves text to paste buffer.
    ^P      Paste. Inserts all text from paste buffer.
    ^Y      Move line up.
    ^B      Move line down.


## File Command Keys 

    ^W      Write to file. Saves the edit buffer to file with a carriage
            return '\r' and a new line '\n' as end of line. Empties the
            undo and paste buffers.
    ESC     Escape. Exits the program if no changes have been made or the
            user does not want to save changes.


## Internals 

* Digital Research CP/M systems
* ANSI terminal
* Aztec C Compiler Vers. 1.06D by Manx Software Systems
* YAZE-AG 2.51.1 by Frank D. Cringle, Michael Haardt and Andreas Gerlich

UE is small - less than 1000 lines of code and around 15 KB binary.

UE is easy to adapt to your needs as a programmer - just edit and rebuild.

* Keyboard mapping is very much a personal preference and is therefore
  made easy to change.
* If you for some reason does not have an ANSI terminal then it should
  not be too difficult to adapt the source code to handle your terminal.
* The sizes of the edit, undo and paste buffers are limited to fit inside
  a Transient Program Area (TPA) of size 62 KB. Adjust with care as the
  program will not run if too much memory is statically allocated.


## License 
_
    Public Domain 2022 (C) by Lars Lindehaven.
    Public Domain 2002 (C) by Terry Loveall.
    Public Domain 1991 by Anthony Howe.  All rights released.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

        Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

        Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
_
