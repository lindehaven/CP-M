# Syntax Highlighter 

_Prints highlighted source code_


## Summary 

Parses and prints a byte buffer with highlighting of syntax for the
selected language.

The language is selected by calling `shl_select_language()` with the
name of a file as argument.

_Example: Select the C language_
    `shl_select_language("EXAMPLE.C")`

The parsing and printing highlighted strings are performed by calling
`shl_highlight()` with appropriate arguments.

* Keywords for the language, ex: `if`, `then`, `else`, `return`
* Single-line comments, ex: `// single-line comment`
* Multi-line comments, ex: `/* multi-line comment */`
* Strings, ex: `"this is a string"`
* Numeric constants, ex: `3.14159`, `0xFACE`, `.01`

Languages need to be defined in shl.c, there is no reading of
language definition files (yet).

Possible to parse and print none, parts or all of the byte buffer so that
the CPU load can be kept to a minimum.

Keeps state of multi-line comments to enable faster parsing in editors,
for example Lean Editor.


## Internals 

* Digital Research CP/M systems
* ANSI terminal
* Aztec C Compiler Vers. 1.06D by Manx Software Systems
* YAZE-AG 2.51.1 by Frank D. Cringle, Michael Haardt and Andreas Gerlich


## License 
_
    Copyright (C) 2017-2022 Lars Lindehaven.

    Work based on the Program Kilo editor, v0.1.1.
    Copyright (c) 2016, Salvatore Sanfilippo <antirez at gmail dot com>

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