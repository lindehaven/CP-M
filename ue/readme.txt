
Micro Editor

Summary -----------------------------------------------------------------------

    Micro Editor (UE) enables easy editing of small source code files. Source
    code files are limited in size to fit into the transient program area (TPA)
    of CP/M.
    
    NOTE! If ^S is pressed repeatedly then screen output may pause.
          Press ^Q once to unpause.

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

    All changes except cut/paste/move line are placed in the undo buffer and
    can be reverted by pressing ^Z (Undo last insert/delete). In case the undo
    buffer becomes full it needs to be emptied by pressing ^Z (Undo last 
    insert/delete) or ^W (Write to file).

Screen Layout -----------------------------------------------------------------

    In case lines are too long to be displayed in full then the rightmost
    column is marked with a '+' to indicate that there are more characters
    beyond the right screen border. To get rid of them, shorten the lines.

    The last row on the bottom of the screen is called the status row.
    It displays:

    *   F   which file that is being edited
    *   E   how many edits that have been made
    *   T   the tab width
    *   C   current column
    *   R   current/last row

    Example:

        F:UE.C                                E:56   T:4 C:9   R:94/463

    If an incremental string search is active then it is shown instead of the
    file name:

    *   S   search string

    Example:

        S:for (i =                            E:119  T:4 C:21  R:382/463

    If an incremental string replacements is active then it is shown instead
    of the file name:

    *   N   new string

    Example:

        N:for (index =                        E:119  T:4 C:21  R:382/463

Navigation --------------------------------------------------------------------

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
    ^L      Search string incrementally. Looks for search string in the edit
            buffer. Wraps when reached end of edit buffer.

Editing -----------------------------------------------------------------------

    ^N      Replace string. Replaces a search string with a new string.
    ^U      Change tab width. Can be 2-8 spaces.
    ^I      Insert tab (Tab).
    ^M      Insert new line (Enter).
    ^G      Delete character to the right (Delete).
    ^H      Delete character to the left (Backspace).
    ^Z      Undo last insert/delete.
    ^O      Cut line. No Undo.
    ^P      Paste line. No Undo.
    ^Y      Move line up. No Undo.
    ^B      Move line down. No Undo.

File commands -----------------------------------------------------------------

    ^W      Write to file. Writes the edit buffer to file with a carriage
            return '\r' and a new line '\n' as end of line.
    ^Q      Quit. Quits immediately if there are no edits made.

Internals ---------------------------------------------------------------------

    *   Digital Research CP/M systems
    *   ANSI terminal
    *   Aztec C Compiler, 8080 Assembler and C Linker Vers. 1.06D
        by Manx Software Systems
    *   Tested on YAZE-AG 2.40.2 by Frank D. Cringle, Michael Haardt
        and Andreas Gerlich.

Changes -----------------------------------------------------------------------

    1.26.2 Jan. 9, 2018. Lars Lindehaven
    *   Added function to replace a search string.

    1.26.1 Jan. 2, 2018. Lars Lindehaven
    *   Fixed column display during search.

    1.26.0 Dec. 28, 2017. Lars Lindehaven
    *   Ported to CP/M , Intel 8080 and Zilog Z80.
    *   Written keyPressed function in Intel 8080 assembly for CP/M.
    *   Made use of ANSI terminals' clear-to-end-of-line.
    *   Changed the search function.
    *   Lines longer than the screen width are visualized with '+'.
    *   Added the undo function.
    *   Added function to move a line up/down.
    *   Added function to cut/paste a line.
    *   Added status row at the bottom of the screen.

    1.25 Sept. 6, 2002. Terry Loveall
    *   Initial release 'OOPS' fix.
    *   Added 'Limitations' and 'Changes' sections, this file.
    *   Corrected 'compile with' command line, this file.
    *   Improved 'compile with' command line for size in ue.c and this
        file.
    *   Picked up a size optimization in function look().
    *   Fixed getch() to ignore PC function keys and to properly accept
        'paste' text from gpm or X terminal.
    *   Changed array 'key[]' from type short to char for reduced size.

    1.24 Sept. 5, 2002. Terry Loveall
    *   Initial release.

    1991. Anthony Howe.
    *   Initial release.

License -----------------------------------------------------------------------

    Public Domain 2017-2018 (C) by Lars Lindehaven.
    Public Domain 2002 (C) by Terry Loveall.
    Public Domain 1991 by Anthony Howe.  All rights released.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

      * Redistributions in binary form must reproduce the above copyright
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
