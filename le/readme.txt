
    Lean Editor -- a small text editor for programmers
   
    Summary ---------------------------------------------------------------
    
        *   Digital Research CP/M systems
        *   Wordstar key mapping
        *   ANSI terminal
        *   Aztec C Compiler, 8080 Assembler and C Linker Vers. 1.06D
            by Manx Software Systems
        *   Tested on YAZE-AG 2.40.2 by Frank D. Cringle, Michael Haardt
            and Andreas Gerlich.

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

    Editing ---------------------------------------------------------------
    
        ^O      Change tab width (2, 4, 6 or 8 spaces)
        ^I      Insert tab
        ^M      Insert CR/LF
        ^G      Delete character to the right
        DEL     Delete character to the right
        ^H      Delete character to the left
        BACK    Delete character to the left

    File commands ---------------------------------------------------------
    
        ^W      Write file (save)
        ^Q      Quit (exit)

    Functionality ---------------------------------------------------------
    
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
        
    Internals -------------------------------------------------------------
    
        LE is written for CP/M systems but can easily be ported to other
        operating systems.

    -----------------------------------------------------------------------

