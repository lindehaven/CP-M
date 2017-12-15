# CP/M

## Background

I wrote lots of code for CP/M in the early 80's. Basic, TurboPascal, C and
assembly. My source code is unfortunately gone with the disks that I stored
it on and with the computers I ran it on; a Spectravideo SV-328, Jet 80 and 
Bondwell BW-2. By 1995 I thought that I would not use CP/M again. UNIX and M$ Windows had taken over.

But when I found and tried out some CP/M emulators I got the urge to write
some of that good old code again - from scratch.

I started with the Binary Editor because I had real good use of that kind of
editor and it should be a given tool in any programmers toolbox. Could not
find any CP/M source code or binary in the web archives. Managed to design,
code, test and debug a decent binary editor in a couple of days. Have already
used it a few times when inspecting files saved by other programs.

Continued with an attempt to port the Kilo editor [1] to CP/M because I wanted
a source code editor with syntax highlighting. But I soon discovered that the
Kilo editor was using too much of the 62KB TPA so I decided to write an editor
that uses less memory. Based most of my design and code on the Text editor [2]
that works well in CP/M. I took away functions that I don't need (new file, open
file, save file as) and added some that I do need (incremental search, move line
up/down). That became the Lean Editor.

Also wrote the Syntax Highlighter that is a variant of the syntax highlighting
from Kilo editor [1]. I focused on reducing the memory requirements because I
want to have as much free memory in TPA as possible if the Syntax Highlighter
is to be linked with a text editor.

While developing the Lean Editor and Syntax Highlighter I found the micro
editor [3] written for *nix. It is small and have the search and undo
functions that is needed in any source code editor. So I ported ue to CP/M to
test it. Works ok.

References:

  [1] Kilo editor (kilo) by Salvatore Sanfilippo (antirez), https://github.com/antirez/kilo
  
  [2] Text editor (te) by Miguel I. García López (MiguelVis), https://github.com/MiguelVis

  [3] Micro Editor (ue) by Anthony Howe and Terry Loveall,       http://web.archive.org/web/20081019042406/http://www.modest-proposals.com/binary/ue.1.25.tgz


## Binary Editor (BE)
   
### Summary 

*   Digital Research CP/M systems
*   ANSI terminal
*   Aztec C Compiler, 8080 Assembler and C Linker Vers. 1.06D by
    Manx Software Systems
*   Tested on YAZE-AG 2.40.2 by Frank D. Cringle, Michael Haardt
    and Andreas Gerlich.

### Functionality 

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

### Internals 

BE is written for CP/M systems but can easily be ported to other
operating systems.

Keyboard mapping can be easily changed without compiling the BE
source code, assembling and linking. There are 13 editor keys that
are located at even adresses 0x0010 - 0x0028 in BE.COM. By editing
BE.COM using itself(!) you can change these key mappings and save
the BE.COM that you want. The help text for key mappings are located
at 0x0030 - 0x00ff and you should edit those too while you are at it.
Save a backup of the original BE.COM before you start editing.


## Aztec C Library (CLIB)

Wrote a few modules that were missing in the Aztec C distribution.

*   'memcpy.c'
*   'strchr.c'
*   'strstr.c'


## Lean Editor (LE)
   
### Summary 

*   Digital Research CP/M systems
*   ANSI terminal
*   Aztec C Compiler, 8080 Assembler and C Linker Vers. 1.06D by
    Manx Software Systems
*   Tested on YAZE-AG 2.40.2 by Frank D. Cringle, Michael Haardt
    and Andreas Gerlich.

### Functionality 

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

### Internals 

LE is based on the Program Text editor (te) v1.08 from 05 Jul 2017.
Copyright (c) 2015-2016 Miguel Garcia / FloppySoftware.
LE is written for CP/M systems.


## Syntax Highlighter (SHL)

### Summary

Parses and prints a byte buffer with highlighting of syntax for the
selected language.

### Functionality 

The language is selected by calling shl_select_language() with the name
of a file as argument, see 'shl.h' for details.
Ex: shl_select_language("EXAMPLE.C") selects the C language.

The parsing and printing highlighted strings are performed by calling
shl_highlight() with appropriate arguments, see 'shl.h' for details.

*   Keywords for the language
    Ex: if, then, else, return
*   Single-line comments
    Ex: // single-line comment
*   Multi-line comments
    Ex: /* multi-line comment */
*   Strings
    Ex: "this is a string"
*   Numeric constants
    Ex: 3.14159, 0xFACE, .01

### Internals 

SHL is based on the syntax highlighting in Kilo editor (kilo) v0.0.1.

Languages need to be defined in 'shl.c', there is no reading of language
definition files (yet).

Possible to parse and print none, parts or all of the byte buffer so that
the CPU load can be kept to a minimum.

Keeps state of multi-line comments to enable faster parsing in editors,
for example Lean Editor.


## Micro Editor (UE)

### Summary 

*   Digital Research CP/M systems
*   ANSI terminal
*   Aztec C Compiler, 8080 Assembler and C Linker Vers. 1.06D by
    Manx Software Systems
*   Tested on YAZE-AG 2.40.2 by Frank D. Cringle, Michael Haardt
    and Andreas Gerlich.

### Functionality 

See ue/README file.

### Internals 

See ue/README file.
