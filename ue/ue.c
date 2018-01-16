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
#define KEY_CUT     ('O' & 0X1F)    /* Cut to end of line */
#define KEY_PASTE   ('P' & 0X1F)    /* Paste */
#define KEY_LMVUP   ('Y' & 0X1F)    /* Move line up */
#define KEY_LMVDN   ('B' & 0X1F)    /* Move line down */
#define KEY_REPL    ('N' & 0X1F)    /* Replace string incrementally */
#define KEY_UNDO    ('Z' & 0X1F)    /* Undo last character insert/delete */
#define KEY_FSAVE   ('W' & 0X1F)    /* Write to file */
#define KEY_FQUIT   ('Q' & 0X1F)    /* Quit */

#define MAX_UNDO    3200
#define UNDO_SIZE   MAX_UNDO*sizeof(U_REC)
#define EDIT_SIZE   32000
#define PASTE_SIZE  1000
#define SRCH_SIZE   32

#define STAT_FILE   "Filename: "
#define STAT_SRCH   "Search for: "
#define STAT_REPL   "Replace with: "
#define STAT_ERR    "!!! "

#define DEF_FNAME   "$$$$$$$$.$$$"
#define MIN_TW      2
#define DEF_TW      8
#define MAX_TW      8
#define MIN_AI      0
#define DEF_AI      1
#define MAX_AI      1
#define MIN_ROWS    10
#define DEF_ROWS    25
#define MAX_ROWS    50
#define MIN_COLS    40
#define DEF_COLS    80
#define MAX_COLS    132

typedef struct {
    int X;
    int Y;
} COORD;

typedef struct {
    char ch;
    char *pos;
} U_REC;

char undobuf[UNDO_SIZE], editbuf[EDIT_SIZE], pastebuf[PASTE_SIZE];
char *editp = editbuf, *endp = editbuf, *pastep = pastebuf;
char *page, *epage, fname[40];
U_REC* undop = (U_REC*)&undobuf;
COORD outxy;
int done = 0, lines = 1, cuts = 0, row = 0, col = 0;
long int eds = 0;
int tw, ai, rows, cols;

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
void inctw();
void ins();
void del();
void rub();
void cut();
void paste();
void lmvup();
void lmvdn();
void undo();
int fread();
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
    KEY_RUB,    KEY_BS,     KEY_CUT,    KEY_PASTE,
    KEY_LMVUP,  KEY_LMVDN,  KEY_UNDO,   KEY_FSAVE,
    KEY_FQUIT,
    '\0'
};

void (*func[])() = {
    rup,        rdn,        clt,        crt, 
    wlt,        wrt,        lbeg,       lend,
    pup,        pdn,        fbeg,       fend, 
    sarstr,     sarstr,     inctw,      del,
    rub,        rub,        cut,        paste,
    lmvup,      lmvdn,      undo,       fsave,
    fquit,
    nop
};

void clrscreen() {
    printf("\x1b[2J");
}

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

void settw(t) int t; {
    tw = t >= MIN_TW && t <= MAX_TW ? t : DEF_TW;
}

void setai(a) int a; {
    ai = a >= MIN_AI && a <= MAX_AI ? a : DEF_AI;
}

void setrows(r) int r; {
    rows = r >= MIN_ROWS && r <= MAX_ROWS ? r : DEF_ROWS;
}

void setcols(c) int c; {
    cols = c >= MIN_COLS && c <= MAX_COLS ? c : DEF_COLS;
}

void put1(ch) int ch; {
    if (ch != 0x00 && ch != 0x1a && ch != '\r') {
        if (ch == '\n') { outxy.X = 0; outxy.Y++; putchar('\n'); }
        else if (++outxy.X < cols-1) putchar(ch & 0x7f);
        if (outxy.X == cols-1) putchar('+');
    }
}

void emitch(ch) int ch; {
    if (ch == '\t') do put1(' '); while (outxy.X % tw) ;
    else put1(ch);
}

char *prevline(p) char *p; {
    while (--p > editbuf && *p != '\n') ;
    return editbuf < p ? ++p : editbuf;
}

char *nextline(p) char *p; {
    while (p < endp && *p++ != '\n') ;
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

void undopfull() {
    status(STAT_ERR, "Undo buffer is full. "); keyPressed();
}

void editbfull() {
    status(STAT_ERR, "Edit buffer is full. "); keyPressed();
}

void pastebfull() {
    status(STAT_ERR, "Paste buffer is full. "); keyPressed();
}

void fwriterr() {
    status(STAT_ERR, "%s could not be written. ", fname); keyPressed();
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
    int i = rows-1;
    while (--i > 0) { page = prevline(page-1); rup(); }
}

void pdn() {
    page = editp = prevline(epage-1);
    while (row-- > 0) rdn();
    epage = endp;
}

void fbeg() {
    editp = editbuf;
}

void fend() {
    epage = editp = endp;
}

int sarins(str, len, ch) char *str; int len; char ch; {
    if (len < SRCH_SIZE-1) {
        str[len] = ch == '\r' ? '\n' : ch;
        str[++len] = 0;
    }
    return len;
}

int sardel(str, len) char *str; int len; {
    if (len > 0) str[--len] = 0;
    return len;
}

void sarstr() {
    static char sstring[SRCH_SIZE] = "", rstring[SRCH_SIZE] = "";
    static int slen = 0, rlen = 0;
    int i, found = 0, replace = 0;
    char ch, *p, *foundp = editp;
    do {
        if (replace) status(STAT_REPL, rstring);
        else status(STAT_SRCH, sstring);
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
                lbeg(); display();
                editp = foundp;
                for (col = 0, p = prevline(editp); p < editp; p++)
                    col += *p == '\t' ? tw - col % tw : 1;
                gotoxy(col+1, row+1);
                invvideo();
                for (i = 0; i < slen; i++) emitch(sstring[i]);
                normvideo();
            } else {
                found = 0;
                fbeg(); display();
            }
        }
    } while (ch != 0x1b && ch != KEY_CLT && ch != KEY_CRT &&
             ch != KEY_RUP && ch != KEY_RDN);
}

void inctw() {
    if (++tw > 8) tw = 2;
}

void ins(ch) char ch; {
    char *p;
    if (endp < editbuf+EDIT_SIZE-1) {
        if ((void*)undop < (void*)editbuf) {
            if (ch == '\r') {
                ins('\n');
                if (ai) {
                    p = editp-1;
                    while (--p > editbuf && *p != '\n') ;
                    while (*++p == '\t' || *p == ' ') ins(*p);
                }
            } else {
                if (ch == '\n') lines++;
                editmove(editp, editp+1, endp-editp);
                undop->ch = *editp = ch & 0x7f;
                undop->pos = editp;
                undop++; eds++;
                editp++;
            }
        } else
            undopfull();
    } else
        editbfull();
}

void del() {
    if (editp < endp) {
        if ((void*)undop < (void*)editbuf) {
            if (*editp == '\n') lines--;
            undop->ch = *editp | 0x80;
            undop->pos = editp;
            undop++; eds++;
            editmove(editp+1, editp, endp-editp);
        } else
            undopfull();
    }
}

void rub() {
    if (editbuf < editp) { clt(); del(); }
}

void cut() {
    while (editp < endp && (void*)undop < (void*)editbuf
           && cuts < PASTE_SIZE) {
        pastep[cuts] = *editp;
        del();
        if (pastep[cuts++] == '\n') break;
    }
    if ((void*)undop >= (void*)editbuf) undopfull();
    if (cuts >= PASTE_SIZE) pastebfull();
}

void paste() {
    int i = 0;
    while (i < cuts && endp < editbuf+EDIT_SIZE-1-cuts
           && (void*)undop < (void*)editbuf)
        ins(pastep[i++]);
    if ((void*)undop >= (void*)editbuf) undopfull();
    if (endp >= editbuf+EDIT_SIZE-1-cuts) editbfull();
    cuts = 0;
}

void lmvup() {
    if (currline() > 1) { lbeg(); cut(); rup(); paste(); rup(); }
}

void lmvdn() {
    if (currline() < lines) { lbeg(); cut(); rdn(); paste(); rup(); }
}

void undo() {
    if ((void*)undop > (void*)undobuf) {
        undop--; eds--; 
        editp = undop->pos;
        if (undop->ch & 0x80) {
            editmove(editp, editp+1, endp-editp);
            *editp = undop->ch & 0x7f;
            if (*editp == '\n') lines++;
        } else {
            if (*editp == '\n') lines--;
            editmove(editp+1, editp, endp-editp);
        }
    }
}

int fread() {
    FILE *fp;
    char ch;
    if (fp = fopen(fname, "r")) {
        ch = fgetc(fp) & 0x7f;
        while (!feof(fp) && endp < editbuf+EDIT_SIZE) {
            if (ch > 0x1f || ch == '\t' || ch == '\n') {
                *endp++ = ch;
                if (ch == '\n') lines++;
            }
            ch = fgetc(fp) & 0x7f;
        }
        fclose(fp);
    }
    if (!fp || endp >= editbuf+EDIT_SIZE)
        return -1;
    else
        return 0;
}

void fsave() {
    FILE *fp;
    char *p = &editbuf;
    if (fp = fopen(fname, "w")) {
        while (p < endp) {
            if (*p != '\n') fputc(*p & 0x7f, fp);
            else { fputc('\r', fp); fputc('\n', fp); }
            p++;
        }
        fclose(fp);
    } else
        fwriterr();
    undop = &undobuf; pastep = &pastebuf; cuts = 0; eds = 0;
}

void fquit() {
    if (eds) {
        status(STAT_ERR, "Quit without saving changes? ");
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
        i = rows-1;
        while (i-- > 1) page = prevline(page-1);
    }
    epage = page;
    gotoxy(0, 1);
    while (1) {
        if (editp == epage) {
            row = i; col = j;
        }
        if (i >= rows-1 || i >= lines || endp <= epage)
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
    while (i++ <= rows-1) {
        clrtoeol();
        gotoxy(1, i);
    }
    status(STAT_FILE, fname);
    gotoxy(col+1, row+1);
}

void statstr(s) char *s; {
    int i = 0;
    while (*s && i < SRCH_SIZE) {
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

void status(stat, str) char *stat, *str; {
    long int u = 100*eds/(MAX_UNDO+MAX_UNDO/99)+1;
    int i;
    if (!eds) u = 0;
    invvideo();
    gotoxy(1, rows); clrtoeol();
    for (i = 0; i < cols; i++) putchar(' ');
    gotoxy(cols-30, rows);
    printf("U:%03d%% T:%1d C:%03d R:%05d/%05d",
           (int)u, tw, col+1, currline(), lines);
    gotoxy(2, rows);
    statstr(stat); statstr(str);
    normvideo();
}

int main(argc, argv) int argc, argv[]; {
    int i;
    char ch, *p;
    if (argc > 1) strcpy(fname, argv[1]); else strcpy(fname, DEF_FNAME);
    if (argc > 2) settw(atoi(argv[2])); else settw(DEF_TW); 
    if (argc > 3) setai(atoi(argv[3])); else setai(DEF_AI);
    if (argc > 4) setrows(atoi(argv[4])); else setrows(DEF_ROWS);
    if (argc > 5) setcols(atoi(argv[5])); else setcols(DEF_COLS);
    if (fread()) {
        fprintf(stderr, "Could not read %s", fname);
        return -1;
    } else {
        clrscreen();
        while (!done) {
            display();
            ch = (char)keyPressed() & 0x7f; 
            for (i = 0; key[i] != ch && key[i] != '\0'; i++) ;
            (*func[i])();
            if (key[i] == '\0' && (ch > 0x1f || ch == '\r' || ch == '\t'))
                ins(ch);
        }
        clrscreen();
        return 0;
    }
}
