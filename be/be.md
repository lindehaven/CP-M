# Binary Editor 

_A small binary editor for programmers_


## Summary

Binary Editor (BE) enables hexadecimal and ASCII editing of binary
files up to 32 KB in size.

CP/M programs starts at address 0x0100 by default so this is also
the address offset used by BE as default. This can be changed by
the user when starting BE.

BE continuously keeps track of the current position and displays
that on the top of the terminal screen together with the total file
size.

Any unsaved edits are marked with inverse video on the terminal
screen. The user must also confirm if unsaved edits shall be saved
or discarded when exiting BE.

___NOTE!___
  If ^S is pressed repeatedly then screen output may pause.
  Press ^Q once to unpause.


## Help Key

    ^J      Show help about key controls.


## Navigation Keys 

    ^E      Cursor one row up.
    ^X      Cursor one row down.
    ^S      Cursor one column left.
    ^D      Cursor one column right.
    ^R      Cursor one page up.
    ^C      Cursor one page down.
    ^T      Cursor to beginning of byte buffer (top).
    ^V      Cursor to end of byte buffer (bottom).


## Editing Keys 

    ^A      Set edit mode HEX.
    ^F      Set edit mode ASCII.
    ^Z      Toggle edit mode (ASCII/HEX).


## File Command Keys 

    ^W      Write buffer to file (save).
    ESC     Quit program.


## Internals 

* Digital Research CP/M systems
* ANSI terminal
* Aztec C Compiler Vers. 1.06D by Manx Software Systems
* YAZE-AG 2.51.1 by Frank D. Cringle, Michael Haardt and Andreas Gerlich
    
Keyboard mapping can be easily changed without compiling the BE
source code, assembling and linking. There are 13 editor keys that
are located at 0x0010 - 0x0028 in BE.COM. By editing BE.COM using
itself(!) you can change these key mappings and save the BE.COM
that you want. The help text for key mappings are located at
0x0030 - 0x00ff and you should edit those too while you are at it.
Save a backup of the original BE.COM before you start editing.


## License 
_
    Copyright (C) 2017-2022 Lars Lindehaven. All rights reserved.

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
