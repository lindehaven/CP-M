/*
    Micro Editor (ue)

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

 */

#include "ctype.h"
#include "stdio.h"

#define TERM_ROWS   25
#define TERM_COLS   80

#define KEY_RUP     ('E' & 0X1F)    /* Cursor one row up */
#define KEY_RDN     ('X' & 0X1F)    /* Cursor one row down */
#define KEY_CLT     ('S' & 0X1F)    /* Cursor one column left */
#define KEY_CRT     ('D' & 0X1F)    /* Cursor one column right */
#define KEY_WLT     ('A' & 0X1F)    /* Cursor one word left */
#define KEY_WRT     ('F' & 0X1F)    /* Cursor one word right */
#define KEY_LBEG    ('J' & 0X1F)    /* Cursor to beginning of line */
#define KEY_LEND    ('K' & 0X1F)    /* Cursor to end of line */
#define KEY_PUP     ('R' & 0X1F)    /* Cursor one page up */
#define KEY_PDN     ('C' & 0X1F)    /* Cursor one page down */
#define KEY_FBEG    ('T' & 0X1F)    /* Cursor to beginning of file */
#define KEY_FEND    ('V' & 0X1F)    /* Cursor to end of file */
#define KEY_SRCH    ('L' & 0X1F)    /* Search string incrementally */
#define KEY_TABW    ('U' & 0X1F)    /* Change tab width */
#define KEY_TAB     ('\t')          /* Insert tab */
#define KEY_LINS    ('\r')          /* Insert new line */
#define KEY_DEL     ('G' & 0X1F)    /* Delete character to the right */
#define KEY_RUB     ('H' & 0X1F)    /* Delete character to the left */
#define KEY_BS      (0X7F)          /* Delete character to the left */
#define KEY_LCUT    ('O' & 0X1F)    /* Cut line */
#define KEY_LPASTE  ('P' & 0X1F)    /* Paste line */
#define KEY_LMVUP   ('Y' & 0X1F)    /* Move line up */
#define KEY_LMVDN   ('B' & 0X1F)    /* Move line down */
#define KEY_REPL    ('N' & 0X1F)    /* Replace string incrementally */
#define KEY_UNDO    ('Z' & 0X1F)    /* Undo last character insert/delete */
#define KEY_FSAVE   ('W' & 0X1F)    /* Write to file */
#define KEY_FQUIT   ('Q' & 0X1F)    /* Quit */

typedef struct {
    int X;
    int Y;
} COORD;

typedef struct {
    char ch;
    char *pos;
} U_REC;

#define UNDO_SIZE   (2000*sizeof(U_REC))
#define EDIT_SIZE   (32*1024-1)
#define CLIP_SIZE   (256)
#define MAX_ROWS    (TERM_ROWS-1)
#define MAX_COLS    (TERM_COLS)
#define MAX_SRLEN   (TERM_COLS-4)
#define STAT_COL    (TERM_COLS-30)
#define STAT_CLR    0x01
#define STAT_POS    0x02
#define STAT_FILE   0x04
#define STAT_SRCH   0x08
#define STAT_REPL   0x10
#define STAT_ERR    0x80

/* Do not change the order of these char buffers. */
char undobuf[UNDO_SIZE];
char editbuf[EDIT_SIZE];
char clipbuf[CLIP_SIZE];

char *editp = editbuf, *endp = editbuf;
char *clipp = clipbuf;
char *page, *epage;
char *filename;
U_REC* undop = (U_REC*)&undobuf;
COORD outxy;
int done = 0, edits = 0, totln = 1;
int row = 0, col = 0;
int tw = 8;
int cliplen = 0;

void rup();
void rdn();
void clt();
void crt();
void wlt();
void wrt();
void lbeg();
void lend();
void pup();
void pdn();
void fbeg();
void fend();
void sarstr();
void tabw();
void ins();
void del();
void rub();
void lcut();
void lpaste();
void lmvup();
void lmvdn();
void undo();
void fsave();
void fquit();
void nop();
void display();
void status();

char key[] = { 
    KEY_RUP,    KEY_RDN,    KEY_CLT,    KEY_CRT,
    KEY_WLT,    KEY_WRT,    KEY_LBEG,   KEY_LEND,
    KEY_PUP,    KEY_PDN,    KEY_FBEG,   KEY_FEND,
    KEY_SRCH,   KEY_REPL,   KEY_TABW,   KEY_DEL,
    KEY_RUB,    KEY_BS,     KEY_LCUT,   KEY_LPASTE,
    KEY_LMVUP,  KEY_LMVDN,  KEY_UNDO,   KEY_FSAVE,
    KEY_FQUIT,
    '\0'
};

void (*func[])() = {
    rup,        rdn,        clt,        crt, 
    wlt,        wrt,        lbeg,       lend,
    pup,        pdn,        fbeg,       fend, 
    sarstr,     sarstr,     tabw,       del,
    rub,        rub,        lcut,       lpaste,
    lmvup,      lmvdn,      undo,       fsave,
    fquit,
    nop
};

void gotoxy(x, y) int x; int y; {
    printf("\x1b[%d;%dH", y, x);
    outxy.X = x; outxy.Y = y;
}

void clrtoeol() {
    printf("\x1b[0K");
    gotoxy(outxy.X, outxy.Y);
}

void invvideo() {
    printf("\x1b[7m");
}

void normvideo() {
    printf("\x1b[27m");
}

#asm
; int keyPressed(void);
    public keyPressed
keyPressed:
    lxi d,253
    mvi c,6
    call 5
    mvi h,0
    mov l,a
    ret
#endasm

void put1(ch) int ch; {
    if (ch != 0x00 && ch != 0x1a && ch != '\r') {
        if (ch == '\n') { outxy.X = 0; outxy.Y++; putchar('\n'); }
        else if (++outxy.X < TERM_COLS-1) putchar(ch & 0x7f);
        if (outxy.X == TERM_COLS-1) putchar('+');
    }
}

void emitch(ch) int ch; {
    if (ch == '\t') do put1(' '); while (outxy.X % tw);
    else put1(ch);
}

char *prevline(p) char *p; {
    while (editbuf < --p && *p != '\n');
    return editbuf < p ? ++p : editbuf;
}

char *nextline(p) char *p; {
    while (p < endp && *p++ != '\n');
    return p < endp ? p : endp;
}

char *adjust(p, column) char *p; int column; {
    int j = 0;
    while (p < endp && *p != '\n' && j < column)
        j += *p++ == '\t' ? tw - j % tw : 1;
    return p;
}

int currline() {
    char *p = editbuf;
    int line = 1;
    while (++p <= editp && p <= endp) if (*(p-1) == '\n') line++;
    return line;
}

int linelen(p) char *p; {
    char *pp = prevline(p);
    char *np = nextline(p);
    return np - pp;
}

void charmove(src, dest, cnt) char *src; char *dest; int cnt; {
    if (src > dest)
        while (cnt--) *dest++ = *src++;
    if (src < dest) {
        src += cnt; dest += cnt;
        while (cnt--) *--dest = *--src;
    }
}

void editmove(src, dest, cnt) char *src; char *dest; int cnt; {
    charmove(src, dest, cnt);
    endp += dest-src;
}

void rup() {
    editp = adjust(prevline(prevline(editp)-1), col);
}

void rdn() {
    editp = adjust(nextline(editp), col);
}

void clt() {
    if (editbuf < editp) --editp;
} 

void crt() {
    if (editp < endp) ++editp;
}

void wlt() {
    while (isspace(*(editp-1)) && editbuf < editp) --editp;
    while (!isspace(*(editp-1)) && editbuf < editp) --editp;
}

void wrt() {
    while (!isspace(*editp) && editp < endp) ++editp;
    while (isspace(*editp) && editp < endp) ++editp;
}

void lbeg() {
    editp = prevline(editp);
}

void lend() {
    editp = nextline(editp);
    clt();
}

void pup() {
    int i = MAX_ROWS;
    while (0 < --i) { page = prevline(page-1); rup(); }
}

void pdn() {
    page = editp = prevline(epage-1);
    while (0 < row--) rdn();
    epage = endp;
}

void fbeg() {
    editp = editbuf;
}

void fend() {
    epage = editp = endp;
}

int sarins(str, len, ch) char *str; int len; char ch; {
    if (len < MAX_SRLEN) {
        str[len] = ch == '\r' ? '\n' : ch;
        str[++len] = 0;
        if (ch > 0x1f) putchar(ch);
        else putchar(' ');
    }
    return len;
}

int sardel(str, len) char *str; int len; {
    if (len > 0) {
        str[--len] = 0;
        putchar('\b');
        putchar(' ');
    }
    return len;
}

void sarstr() {
    static char sstring[MAX_SRLEN+1] = "", rstring[MAX_SRLEN+1] = "";
    static int slen = 0, rlen = 0;
    int i, found = 0, replace = 0;
    char ch, *p, *foundp = editp;
    do {
        if (replace) status(STAT_CLR|STAT_POS|STAT_REPL, rstring);
        else status(STAT_CLR|STAT_POS|STAT_SRCH, sstring);
        ch = (char)keyPressed() & 0x7f;
        if (ch == KEY_RUB || ch == KEY_BS) {
            if (replace) rlen = sardel(rstring, rlen);
            else slen = sardel(sstring, slen);
        } else if (ch > 0x1f || ch == '\r' || ch == '\t') {
            if (replace) rlen = sarins(rstring, rlen, ch);
            else slen = sarins(sstring, slen, ch);
        }
        if (slen > 0) {
            if (found && replace && ch == KEY_REPL) {
                for (i = 0; i < slen; i++) del();
                for (i = 0; i < rlen; i++) ins(rstring[i]);
                crt();
            } else if (!replace && ch == KEY_REPL) {
                replace = 1;
            } else if (ch == KEY_SRCH) {
                replace = 0;
                crt();
            }
            while (editp < endp-1 && strncmp(editp, sstring, slen))
                crt();
            if (editp < endp-1) {
                found = 1;
                foundp = page = editp;
                epage = editp+1;
                lbeg();
                display();
                editp = foundp;
                for (col = 0, p = prevline(editp); p < editp; p++)
                    col += *p == '\t' ? tw - col % tw : 1;
                gotoxy(col+1, row+1);
                invvideo();
                for (i = 0; i < slen; i++) emitch(sstring[i]);
                normvideo();
            } else {
                found = 0;
                fbeg();
                display();
            }
        }
    } while (ch != 0x1b && ch != KEY_CLT && ch != KEY_CRT &&
             ch != KEY_RUP && ch != KEY_RDN);
}

void tabw() {
    if (++tw > 8) tw = 2;
}


void ins(ch) char ch; {
    if (endp < editbuf+EDIT_SIZE-1 && (void*)undop < (void*)editbuf) {
        editmove(editp, editp+1, endp-editp);
        *editp = ch == '\r' ? '\n' : ch;
        if (*editp++ == '\n') totln++;
        undop->ch = editp[-1] & 0x7f;
        undop->pos = editp-1;
        undop++; edits++;
    } else {
        status(STAT_CLR|STAT_ERR, "Buffer is full! Undo or save. ");
        keyPressed();
    }
}

void del() {
    if (editp < endp) {
        if ((void*)undop < (void*)editbuf) {
            if (*editp == '\n') totln--;
            undop->ch = *editp | 0x80;
            undop->pos = editp;
            undop++; edits++;
            editmove(editp+1, editp, endp-editp);
        } else {
            status(STAT_CLR|STAT_ERR,"Buffer is full! Undo or save. ");
            keyPressed();
        }
    }
}

void rub() {
    if (editbuf < editp) { clt(); del(); }
}

void lcut() {
    int len = linelen(editp);
    if (len <= CLIP_SIZE) {
        lbeg();
        cliplen = len;
        charmove(editp, clipp, cliplen);
        editmove(editp+cliplen, editp, endp-editp);
        edits++;
    }
}

void lpaste() {
    if (cliplen) {
        lbeg();
        editmove(editp, editp+cliplen, endp-editp);
        charmove(clipp, editp, cliplen);
        cliplen = 0;
    }
}

void lmvup() {
    lcut();
    rup();
    lpaste();
}

void lmvdn() {
    lcut();
    rdn();
    lpaste();
}

void undo() {
    if ((void*)undop > (void*)undobuf) {
        undop--; edits--; 
        editp = undop->pos;
        if (undop->ch & 0x80) {
            editmove(editp, editp+1, endp-editp);
            *editp = undop->ch & 0x7f;
            if (*editp == '\n') totln++;
        } else {
            if (*editp == '\n') totln--;
            editmove(editp+1, editp, endp-editp);
        }
    }
}

void fread() {
    FILE *fp;
    char ch;
    if (fp = fopen(filename, "r")) {
        ch = fgetc(fp) & 0x7f;
        while (!feof(fp) && endp < editbuf+EDIT_SIZE) {
            if (ch > 0x1f || ch == '\t' || ch == '\n') {
                *endp++ = ch;
                if (ch == '\n') totln++;
            }
            ch = fgetc(fp) & 0x7f;
        }
        fclose(fp);
    } else {
        fprintf(stderr, "Cannot read %s!", filename);
        done = 1;
    }
    if (endp >= editbuf+EDIT_SIZE) {
        fprintf(stderr, "File %s is too large!", filename);
        done = 1;
    }
}

void fsave() {
    FILE *fp;
    char *p = &editbuf;
    if (fp = fopen(filename, "w")) {
        while (p < endp) {
            if (*p != '\n') fputc(*p & 0x7f, fp);
            else { fputc('\r', fp); fputc('\n', fp); }
            p++;
        }
        fclose(fp);
    } else {
        status(STAT_CLR|STAT_ERR, "Cannot write to file! ");
        keyPressed();
    }
    undop = &undobuf; edits = 0;
}

void fquit() {
    if (edits) {
        status(STAT_CLR|STAT_ERR, "Quit without saving changes? ");
        if (toupper(keyPressed()) == 'Y') done = 1;
    } else
        done = 1;
}

void nop() {
}

void display() {
    int i = 0, j = 0;
    if (editp < page)
        page = prevline(editp);
    if (epage <= editp) {
        page = editp; 
        i = MAX_ROWS;
        while (1 < i--) page = prevline(page-1);
    }
    epage = page;
    gotoxy(0, 1);
    while (1) {
        if (editp == epage) {
            row = i; col = j;
        }
        if (i >= MAX_ROWS || i >= totln || endp <= epage)
            break;
        if (*epage == '\n') {
            ++i; j = 0;
            clrtoeol();
        }
        if (*epage != '\r') {
            emitch(*epage);
            j += *epage == '\t' ? tw - j % tw : *epage == '\n' ? 0 : 1;
        }
        ++epage;
    }
    i = outxy.Y;
    while (i++ <= MAX_ROWS) {
        clrtoeol();
        gotoxy(1, i);
    }
    status(STAT_CLR|STAT_POS|STAT_FILE, filename);
    gotoxy(col+1, row+1);
}

void statstr(s) char *s; {
    int i = 0;
    while (*s && i < MAX_SRLEN) {
        if (*s > 0x1f) {
            putchar(*s); i += 1;
        } else if (*s == '\t') {
            putchar('\\'); putchar('t'); i += 2;
        } else if (*s == '\n') {
            putchar('\\'); putchar('n'); i += 2;
        }
        s++;
    }
}

void status(mask, str) int mask; char *str; {
    int i;
    invvideo();
    if (mask & STAT_CLR) {
        gotoxy(1, TERM_ROWS); clrtoeol();
        for (i = 0; i < TERM_COLS; i++) putchar(' ');
    }
    if (mask & STAT_POS) {
        i = currline();
        gotoxy(STAT_COL, TERM_ROWS);
        printf("E:%d T:%d C:%d R:%d/%d", edits, tw, col+1, i, totln);
     }
    if (mask & STAT_FILE) {
        gotoxy(2, TERM_ROWS); statstr("F:"); statstr(str);
    }
    if (mask & STAT_SRCH) {
        gotoxy(2, TERM_ROWS); statstr("S:"); statstr(str);
    }
    if (mask & STAT_REPL) {
        gotoxy(2, TERM_ROWS); statstr("N:"); statstr(str);
    }
    if (mask & STAT_ERR) {
        gotoxy(2, TERM_ROWS); statstr(str);
    }
    normvideo();
}

int main(argc, argv) int argc; char **argv; {
    int i;
    char ch, *p;
    if (argc < 2)
        return 2;
    filename = *++argv;
    fread();
    while (!done) {
        display();
        ch = (char)keyPressed() & 0x7f; 
        for (i = 0; key[i] != ch && key[i] != '\0'; i++)
            ;
        (*func[i])();
        if (key[i] == '\0' && (ch > 0x1f || ch == '\r' || ch == '\t'))
            ins(ch);
    }
    gotoxy(1, TERM_ROWS+1);
    return 0;
}
