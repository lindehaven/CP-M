# CP/M

![CP/M Inside](https://github.com/lindehaven/CP-M/blob/master/images/cpm_inside.png)

From Wikipedia:

> CP/M, originally standing for Control Program/Monitor and later Control
> Program for Microcomputers, is a mass-market operating system created for
> Intel 8080/85-based microcomputers by Gary Kildall of Digital Research, Inc.
> Initially confined to single-tasking on 8-bit processors and no more than 64
> kilobytes of memory, later versions of CP/M added multi-user variations and
> were migrated to 16-bit processors.


![Digital Research](https://github.com/lindehaven/CP-M/blob/master/images/digital_research.png)


## Background

I wrote lots of code for CP/M in the early 80's. Basic, TurboPascal, C and
assembly. My source code is unfortunately gone with the disks that I stored
it on and with the computers I ran it on; a Spectravideo SV-328, a Jet 80 and
a Bondwell BW-2. By 1995 I thought that I would not use CP/M again. UNIX and
M$ Windows had taken over.

But when I found and tried out some CP/M emulators I got the urge to write
some of that good old code again.

I started with the Binary Editor because I had real good use of that kind of
editor and it should be a given tool in any programmers toolbox. Could not
find any CP/M source code or binary in the web archives. Managed to design,
code, test and debug a decent binary editor in a couple of days. Have already
used it a few times when inspecting files saved by other programs.

Continued with an attempt to port the Kilo editor [1] to CP/M because I wanted
a source code editor with syntax highlighting. But I soon discovered that the
Kilo editor was using too much of the 62 KB TPA so I decided to write an editor
that uses less memory.

Also wrote the Syntax Highlighter that is a variant of the syntax highlighting
from Kilo editor [1]. I focused on reducing the memory requirements because I
want to have as much free memory in TPA as possible if the Syntax Highlighter
is to be linked with a text editor.

While developing the Syntax Highlighter I found the micro editor [2] written
for Linux. It is small and lean so I ported it to CP/M. Added undo, added
replace, added auto-indentation and syntax highlighting. Working fine.

Although the micro editor works fine in a CP/M emulator, it is way too slow for
use on an actual Intel 8080 or Zilog Z80 running on a few MHz. So, my pursuit
for a faster code editor with syntax highlighting continues.

At the time of writing, I'm working on extending a fork of the TED editor [3]
with Zilog Z80 assembly syntax highlighting - in Zilog Z80 assembly of course.


## References

  [1] Kilo editor (kilo) by Salvatore Sanfilippo
      ([antirez](https://github.com/antirez/kilo))

  [2] Micro Editor by Anthony Howe and Terry Loveall
      ([ue](http://web.archive.org/web/20081019042406/http://www.modest-proposals.com/binary/ue.1.25.tgz)) 

  [3] TED editor by Hector Peraza
      ([hperaza](https://github.com/hperaza/TED))

## Binary Editor (BE)

![BE](https://github.com/lindehaven/CP-M/blob/master/images/be1.png)

### Summary

Binary Editor (BE) enables hexadecimal and ASCII editing of binary files up to
32 KB in size.

CP/M programs starts at address `0x0100` by default so this is also the address
offset used by BE as default. This can be changed by the user when starting BE.

BE continuously keeps track of the current position and displays that on the
top of the terminal screen together with the total file size.

Any unsaved edits are marked with inverse video on the terminal screen. The
user must also confirm if unsaved edits shall be saved or discarded when
exiting BE.

Keyboard mapping can be easily changed without compiling the BE source code,
assembling and linking. There are 13 editor keys that are located at even
adresses `0x0010 - 0x0028` in `BE.COM`. By editing `BE.COM` using itself(!)
you can change these key mappings and save the `BE.COM` that you want. The
help text for key mappings are located at `0x0030 - 0x00ff` and you should
edit those too while you are at it. Save a backup of the original `BE.COM`
before you start editing.

See `be/be.md` or `be/be.m2a` for details.

### Internals

Copyright (c) 2017-2022 Lars Lindehaven.

* Digital Research CP/M systems
* ANSI terminal
* Aztec C Compiler Vers. 1.06D by Manx Software Systems
* YAZE-AG 2.51.1 by Frank D. Cringle, Michael Haardt and Andreas Gerlich


## Aztec C Library (CLIB)

A few modules that were missing in the Aztec C distribution.

* `memcpy.c`
* `strchr.c`
* `strstr.c`


## Markdown to ANSI (M2A)

### Summary

M2A converts and prints markdown code to ANSI screen codes. I wrote it to get
better readability of markdown files on an ANSI terminal. I use it to convert
my markdown files and supply the ANSI-coded files for my CP/M programs.
I give the ANSI-coded files the extension `.M2A`.
Example: `M2A.COM < M2A.MD > M2A.M2A`

The ANSI-code files then looks ok when dumping them on an ANSI screen.
Example: `TYPE M2A.M2A`

M2A supports the following markdown:

* Headings
* Block quotes
* Code blocks
* Unordered lists
* Inline code
* Emphasized text


### Internals

Public domain (C) 2022 Lars Lindehaven

* Digital Research CP/M systems
* ANSI terminal
* Aztec C Compiler Vers. 1.06D by Manx Software Systems
* YAZE-AG 2.51.1 by Frank D. Cringle, Michael Haardt and Andreas Gerlich

### Summary

See `m2a/m2a.md` or `m2a/m2a.m2a` for details.


## Screen

Small utility to set background and foreground colors on an ANSI terminal.
Written in C.


## Syntax Highlighter (SHL)

### Summary

Parses and prints a byte buffer with highlighting of syntax for the selected
language. The language is selected by calling `shl_select_language()` with the
name of a file as argument, see `shl/shl.h` for details.
Example: `shl_select_language("EXAMPLE.C")` selects the C language.

The parsing and printing highlighted strings are performed by calling
`shl_highlight()` with appropriate arguments, see `shl/shl.h` for details.

* Keywords for the language, ex: `if`, `then`, `else`, `return`
* Single-line comments, ex: `// single-line comment`
* Multi-line comments, ex: `/* multi-line comment */`
* Strings, ex: `"this is a string"`
* Numeric constants, ex: `3.14159`, `0xFACE`, `.01`

Languages need to be defined in `shl/shl.c`, there is no reading of language
definition files (yet).

Possible to parse and print none, parts or all of the byte buffer so that the
CPU load can be kept to a minimum.

Keeps state of multi-line comments to enable faster parsing in editors, for
example Lean Editor.

### Internals

Copyright (C) 2017-2022 Lars Lindehaven

Work based on the Program Kilo editor, v0.1.1.
Copyright (c) 2016, Salvatore Sanfilippo <antirez at gmail dot com>

## Micro Editor (UE)

![UE](https://github.com/lindehaven/CP-M/blob/master/images/ue1.png)

### Summary

See `ue/ue.md` or `ue/ue.m2a` for details.

### Internals

Public Domain 2017-2022 (C) by Lars Lindehaven.

* Digital Research CP/M systems
* ANSI terminal
* Aztec C Compiler Vers. 1.06D by Manx Software Systems
* YAZE-AG 2.51.1 by Frank D. Cringle, Michael Haardt and Andreas Gerlich

Work based on the Micro editor, v1.25 by Terry Loveall.
Public Domain 2002 (C) by Terry Loveall.
Public Domain 1991 by Anthony Howe.  All rights released.

See `ue/ue.md` or `ue/ue.m2a` for details.
