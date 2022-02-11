# Markdown to ANSI (M2A)

## Summary 

M2A converts and prints markdown code to ANSI screen codes. I wrote it to get
better readability of markdown files on an ANSI terminal. I use it to convert
my markdown files and supply the ANSI-coded files for my CP/M programs.
I give the ANSI-coded files the extension `.M2A`.

_Example_
  `M2A.COM < M2A.MD > M2A.M2A`

The ANSI-code files then looks ok when dumping them on an ANSI screen.
_Example_
  `TYPE M2A.M2A`

M2A supports the following markdown:

* Headings
* Block quotes
* Code blocks
* Unordered lists
* Inline code
* Emphasized text


## Internals 

* Digital Research CP/M systems
* ANSI terminal
* Aztec C Compiler Vers. 1.06D by Manx Software Systems
* YAZE-AG 2.51.1 by Frank D. Cringle, Michael Haardt and Andreas Gerlich

## License 
_
    Copyright (C) 2022 Lars Lindehaven. All rights reserved.

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
