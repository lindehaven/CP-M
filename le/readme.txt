
    Lean Editor -- a small text editor for programmers
    
    Summary ---------------------------------------------------------------
    
        Lean Editor (LE) enables easy editing of small source code files.
        Source code files are limited to 2000 lines with lengths less than
        80 characters. The reasons are to simplify the design of LE and
        that source code files should be small.
        
        To give more help for programmers, LE can be linked with the
        Syntax Highlighter (SHL) module to highlight source code while
        editing. This feature has been tested in the YAZE-AG simulator
        running on an Intel i5 at 2.6 GHz. However, the performance will
        probably be too low in a real CP/M environment with a Zilog Z80 or
        Intel Intel 8080 CPU running at a few MHz. Just saying.

    Screen Layout ---------------------------------------------------------
    
        The first row on top of the screen is called the system line. It
        displays:
        *   F   which file that is being edited
        *   P   how many lines there are in the paste buffer
        *   E   how many edits that have been made
        *   T   the tab width
        *   R   current/last row
        *   C   current/last column
        Example:
            `F:LE.C                  P04  E00056  T4  R0094/1000  C09/68`

        If an incremental string search is active then it is shown
        instead of the file name:
        *   S   incremental search string
        Example:
            `S:for (i =              P00  E00119  T4  R0382/1000  C21/52`

    Navigation ------------------------------------------------------------
    
        ^E      Cursor one row up
        ^X      Cursor one row down
        ^S      Cursor one column left
        ^D      Cursor one column right
        ^A      Cursor one word left
        ^F      Cursor one word right
        ^J      Cursor to beginning of line
        ^K      Cursor to end of line
        ^R      Cursor one page up
        ^C      Cursor one page down
        ^T      Cursor to beginning of file 
        ^V      Cursor to end of file
        ^Z      Center current row
        ^L      Search string incrementally (look)

    Editing ---------------------------------------------------------------
    
        ^U      Change tab width (1-8 spaces)
        ^I      Insert tab width spaces
        ^M      Insert new line
        ^G      Delete character to the right
        DEL     Delete character to the right
        ^H      Delete character to the left
        BACK    Delete character to the left
        ^O      Move line to paste buffer
        ^P      Insert line(s) from paste buffer
        ^Y      Move line up
        ^B      Move line down

    File commands ---------------------------------------------------------
    
        ^W      Write file (save)
        ^Q      Quit (exit)

    Internals -------------------------------------------------------------
    
        *   Digital Research CP/M systems
        *   Wordstar key mapping
        *   ANSI terminal
        *   Aztec C Compiler, 8080 Assembler and C Linker Vers. 1.06D
            by Manx Software Systems
        *   Tested on YAZE-AG 2.40.2 by Frank D. Cringle, Michael Haardt
            and Andreas Gerlich.

    License ---------------------------------------------------------------

        Copyright (C) 2017 Lars Lindehaven.
        
        Work based on the Program Text editor (te) v1.08 from 05 Jul 2017.
        Copyright (c) 2015-2016 Miguel Garcia / FloppySoftware.
        
        This program is free software; you can redistribute it and/or
        modify it under the terms of the GNU General Public License as
        published by the ree Software Foundation; either version 2, or
        (at your option) any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with this program; if not, write to the Free Software
        Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
        
        See the file 'copying.txt' for more details.

