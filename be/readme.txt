
    Binary Editor -- a small binary editor for programmers
   
    Summary ---------------------------------------------------------------
    
        *   Digital Research CP/M systems
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
        ^R      Cursor one page up
        ^C      Cursor one page down
        ^T      Cursor to beginning of byte buffer (top)
        ^V      Cursor to end of byte buffer (bottom)

    Editing ---------------------------------------------------------------
    
        ^A      Set edit mode HEX
        ^F      Set edit mode ASCII
        ^Z      Toggle edit mode (ASCII/HEX)

    File commands ---------------------------------------------------------
    
        ^W      Write buffer to file (save)
        ^Q      Quit

    Functionality ---------------------------------------------------------
    
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
        
    Internals -------------------------------------------------------------
    
        BE is written for CP/M systems but can easily be ported to other
        operating systems.
        
        Keyboard mapping can be easily changed without compiling the BE
        source code, assembling and linking. There are 13 editor keys that
        are located at 0x0010 - 0x0028 in BE.COM. By editing BE.COM using
        itself(!) you can change these key mappings and save the BE.COM
        that you want. The help text for key mappings are located at
        0x0030 - 0x00ff and you should edit those too while you are at it.
        Save a backup of the original BE.COM before you start editing.
        
    -----------------------------------------------------------------------
    
