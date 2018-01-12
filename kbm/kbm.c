/*
 * Keyboard Mapping
 *
 * Copyright (C) 2017-2018 Lars Lindehaven. All rights reserved.
 */

main(argc, argv) int argc, argv[];
{
    int c;
    printf("Press key to show keyboard mapping. Press Ctrl+C (^C) to quit.\n");
    while ((c = keyInAsm()) != 3) {
        if (c < 0x20) {
            putchar('^');
            putchar(c + 'A' - 1);
        }
        else if (c > 0x7f) {
            printf("%x", c);
        }
        else {
            putchar(c);
        }
        if ((c == 0x0a) || (c == 0x0d)){
            putchar(0x0a);
            putchar(0x0d);
        }
        else {
            putchar(' ');
        }
    }
}

#asm
; int keyInAsm(void);
;   Get character from the keyboard.
        public      keyInAsm
keyInAsm:
        lhld        1           ;ld      hl,(1)
        lxi         d,6         ;ld      de,6
        dad         d           ;add     hl,de
        lxi         d,keyin2    ;ld      de,keyin2
        push        d           ;push    de
        pchl                    ;jp      (hl)
keyin2:
        mvi         h,0         ;ld      h,0
        mov         l,a         ;ld      l,a
        ret                     ;ret

; void charOutAsm(int ch);
;   Output character to the terminal screen.
        public      charOutAsm
charOutAsm:
        mov         a,l         ;ld       a,l
        cpi         10          ;cp       10
        jnz         charout2    ;jr       nz,charout2
        call        charout2    ;call     charout2
        mvi         l,13        ;ld       l,13
charout2:
        mov         c,l         ;ld       c,l
        lhld        1           ;ld       hl,(1)
        lxi         d,9         ;ld       de,9
        dad         d           ;add      hl,de
        pchl                    ;jp       (hl)
#endasm
