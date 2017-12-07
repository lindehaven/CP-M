/*
 *  Lean Editor -- a small text editor for programmers
 *
 *  Copyright (C) 2017 Lars Lindehaven <lars dot lindehaven at gmail dot com>
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* INCLUDES --------------------------------------------------------------- */

/* Mandatory */
#include "ctype.h"
#include "stdio.h"
#include "le.h"

/* Optional syntax highlighting requires the SHL library and more memory. */
#ifdef SHL
#include "shl.h"
#endif

/* DEFINITIONS ------------------------------------------------------------ */

#define PROG_NAME   "Lean Editor"
#define PROG_AUTH   "Lars Lindehaven"
#define PROG_VERS   "v0.1.5 2017-12-07"
#define PROG_SYST   "CP/M"
#define PROG_TERM   "ANSI Terminal"

#define fputc       putc

#define MAX_FNAME   22
#define MAX_LINES   2000

#define SY_ROW1     0
#define SY_ROWS     1
#define SY_COLS     (TERM_COLS)
#define SY_COLM     0
#define SY_COLF     0
#define SY_COLI     (SY_COLS - 35)

#define ED_ROW1     SY_ROWS
#define ED_ROWS     ((TERM_ROWS) - ED_ROW1)
#define ED_COLS     (TERM_COLS)
#define MAX_ED_COLS ((TERM_COLS) - 1)
#define ED_TABS     (TERM_TABS)
#define MAX_ED_TABS 8


/* GLOBALS ---------------------------------------------------------------- */

/* File */
char            fname[MAX_FNAME];

/* Lines */
unsigned int    *lp_arr;
int             lp_tot;
int             lp_cur;
char            *ln_dat;
char            *ln_clp;
unsigned int    ln_eds;

/* Editor screen */
int             ed_row;
int             ed_col;
int             ed_tab;
int             ed_sys_msg;

/* Syntax highlighting */
#ifdef SHL
int             ed_shl = SHL_FAIL;
#endif


/* PROGRAM ---------------------------------------------------------------- */

int main(argc, argv) int argc, argv[]; {
    int i;

    if (edInitialize())
        return -1;
    if (argc != 2) {
        fprintf(stderr, "Usage: le [filename]");
        return -1;
    } else {
        if (strlen(argv[1]) >= MAX_FNAME) {
            fprintf(stderr, "Filename too long!");
            return -1;
        } else {
            strcpy(fname, argv[1]);
            if (fileRead(fname))
                return -1;
        }
    }
#ifdef SHL
    ed_shl = shl_set_language(fname);
    if  (ed_shl != SHL_FAIL)
        shl_highlight(lp_arr[0], lp_arr[lp_tot], lp_arr[lp_tot], 0);
#endif
    scrClr();
    edLoop();
    scrClr();
    return 0;
}


/* EDITING ---------------------------------------------------------------- */

/* Main editor loop */
edLoop() {
    int ch;

    sysMsg(NULL);
    sysPutFileName();
    edUpdAll();
    while (1) {
        if (ed_sys_msg)
            ed_sys_msg = 0;
        ch = edGetLine();
        switch (ch) {
            case KEY_RUP :
                rowUp();
                break;
            case KEY_RDN :
                rowDown();
                break;
            case KEY_PUP :
                pageUp();
                break;
            case KEY_PDN :
                pageDown();
                break;
            case KEY_FBEG :
                toBOF();
                break;
            case KEY_FEND :
                toEOF();
                break;
            case KEY_LNEW :
                insCR();
                break;
            case KEY_DEL :
                delChrRight();
                break;
            case KEY_RUB :
            case KEY_BS :
                delChrLeft();
                break;
            case KEY_FSAVE :
                fileWrite(fname);
                ln_eds = 0;
                edUpdAll();
                break;
            case KEY_FQUIT :
                if (!ln_eds)
                    return 0;
                else if (sysMsgConf("Discard changes (Y/N)? "))
                    return 0;
                else {
                    sysPutFileName();
                    edUpdAll();
                }
                break;
        }
    }
}

/* Edit current line */
edGetLine() {
    int i, t, ch, len, run, upd_lin, upd_sys, upd_pos, spc, old_len;
    char *buf;

    strcpy(ln_dat, lp_arr[lp_cur]);
    len = old_len = strlen(ln_dat);
    run = upd_sys = upd_pos = 1;
    upd_lin = spc = 0;
    if (ed_col > len)
        ed_col = len;
    while (run) {
        if (upd_lin) {
            upd_lin = 0;
            putstr(ln_dat + ed_col);
            if (spc) {
                putchar(' ');
                spc = 0;
            }
        }
        if (upd_sys) {
            upd_sys = 0;
            scrChrInverted();
            scrPosCur(SY_ROW1, SY_COLI);
            printf("E:%05d  ", ln_eds);
            printf("T:%d  ", ed_tab);
            printf("R:%04d/%04d  ", lp_cur + 1, lp_tot);
            printf("C:%02d/%02d", ed_col + 1, len);
            scrChrNormal();
        }
        if (upd_pos) {
            upd_pos = 0;
            scrPosCur(ED_ROW1 + ed_row, ed_col);
        }
        ch = keyPressed();
        switch (ch) {
            case KEY_RUP :
                if (lp_cur)
                    run = 0;
                ++upd_pos;
                break;
            case KEY_RDN :
                if (lp_cur < lp_tot - 1)
                    run = 0;
                ++upd_pos;
                break;
            case KEY_CLT :
                if (ed_col) {
                    --ed_col;
                    ++upd_sys;
                } else if (lp_cur) {
                    ed_col = 0x7fff;
                    ch = KEY_RUP;
                    run = 0;
                }
                ++upd_pos;
                break;
            case KEY_CRT :
                if (ed_col < len) {
                    ++ed_col;
                    ++upd_sys;
                } else if (lp_cur < lp_tot - 1) {
                    ch = KEY_RDN;
                    ed_col = run = 0;
                }
                ++upd_pos;
                break;
            case KEY_WLT :
                if (ed_col) {
                    if ((ln_dat[ed_col] & 0x7f) != ' ' &&
                        (ln_dat[ed_col - 1] & 0x7f) == ' ')
                        --ed_col;
                    while (ed_col && (ln_dat[ed_col] & 0x7f) == ' ')
                        --ed_col;
                    while (ed_col && (ln_dat[ed_col] & 0x7f) != ' ') {
                        if ((ln_dat[--ed_col] & 0x7f) == ' ') {
                            ++ed_col;
                            break;
                        }
                    }
                    ++upd_sys;
                } else if (lp_cur) {
                    ed_col = 0x7fff;
                    ch = KEY_RUP;
                    run = 0;
                }
                ++upd_pos;
                break;
            case KEY_WRT :
                if (ed_col >= len && lp_cur < lp_tot - 1) {
                    ch = KEY_RDN;
                    ed_col = run = 0;
                } else {
                    while (ln_dat[ed_col] && (ln_dat[ed_col] & 0x7f) != ' ')
                        ++ed_col;
                    while ((ln_dat[ed_col] & 0x7f) == ' ')
                        ++ed_col;
                    ++upd_sys;
                    ++upd_pos;
                }
                break;
            case KEY_LBEG :
                if (ed_col) {
                    ed_col = 0;
                    ++upd_sys;
                }
                ++upd_pos;
                break;
            case KEY_LEND :
                if (ed_col != len) {
                    ed_col = len;
                    ++upd_sys;
                }
                ++upd_pos;
                break;
            case KEY_PUP :
            case KEY_FBEG :
                if (lp_cur || ed_col)
                    run = 0;
                ++upd_pos;
                break;
            case KEY_PDN :
            case KEY_FEND :
                if (lp_cur < lp_tot - 1 || ed_col != len)
                    run = 0;
                ++upd_pos;
                break;
            case KEY_TABW :
                ed_tab += 2;
                if (ed_tab > MAX_ED_TABS)
                    ed_tab = 2;
                ++upd_sys;
                ++upd_pos;
                break;
            case KEY_TAB :
                t = ed_tab - ed_col % ed_tab;
                if (len + t < MAX_ED_COLS) {
                    while (t--) {
                        for (i = len; i > ed_col; --i) {
                            putchar(' ');
                            ln_dat[i] = ln_dat[i - 1];
                        }
                        ln_dat[ed_col++] = ' ';
                        ln_dat[++len] = 0;
                    }
                    ++ln_eds;
                }
                break;
            case KEY_LNEW :
                if (lp_tot < MAX_LINES) {
                    run = 0;
                    ++ln_eds;
                }
                break;
            case KEY_DEL :
                if (ed_col < len) {
                    strcpy(ln_dat + ed_col, ln_dat + ed_col + 1);
                    --len;
                    ++upd_sys;
                    ++upd_lin;
                    ++spc;
                    ++ln_eds;
                } else if (lp_cur < lp_tot -1) {
                    ++ln_eds;
                    run = 0;
                }
                ++upd_pos;
                break;
            case KEY_RUB :
            case KEY_BS :
                if (ed_col) {
                    strcpy(ln_dat + ed_col - 1, ln_dat + ed_col);
                    --ed_col;
                    --len;
                    ++upd_sys;
                    ++upd_lin;
                    ++spc;
                    ++ln_eds;
                    putchar('\b');
                } else if (lp_cur) {
                    ++ln_eds;
                    run = 0;
                }
                ++upd_pos;
                break;
            case KEY_FSAVE :
            case KEY_FQUIT :
                run = 0;
                break;
            default :
                if (len < MAX_ED_COLS && ch >= ' ') {
                    putchar(ch & 0x7f);
                    for (i = len; i > ed_col; --i) {
                        ln_dat[i] = ln_dat[i - 1];
                    }
                    ln_dat[ed_col++] = ch & 0x7f;
                    ln_dat[++len] = 0;
                    ++upd_lin;
                    ++upd_sys;
                    ++ln_eds;
                }
                ++upd_pos;
                break;
        }
    }
    if (len == old_len) {
        strcpy(lp_arr[lp_cur], ln_dat);
    } else if (!(buf = malloc(len + 1))) {
        errMsgMemory();
    } else {
        strcpy(buf, ln_dat);
        free(lp_arr[lp_cur]);
        lp_arr[lp_cur] = buf;
        edUpdAll();
    }
    return ch;
}

/* Initialize global variables */
int edInitialize() {
    int i;

    ln_dat = malloc(MAX_ED_COLS + 2);
    ln_clp = malloc(MAX_ED_COLS + 1);
    lp_arr = malloc(MAX_LINES * 2);
    if (!ln_dat || !ln_clp || !lp_arr) {
        fprintf(stderr, "Not enough memory!");
        return -1;
    }
    *ln_clp = ln_eds = 0;
    for (i = 0; i < MAX_LINES; ++i)
        lp_arr[i] = NULL;
    lp_tot = lp_cur = 0;
    ed_row = ed_col = 0;
    ed_tab = ED_TABS;
    return 0;
}

/* Return line number of first line printed on the editor screen */
edGetFirstLineNumber() {
    return lp_cur - ed_row;
}

/* Return line number of last line printed on the editor screen */
edGetLastLineNumber() {
    int last;

    last = edGetFirstLineNumber() + ED_ROWS - 1;
    return last >= lp_tot - 1 ? lp_tot - 1 : last;
}

/* Clear the editor screen */
edClr() {
    int i;

    for (i = 0; i < ED_ROWS; ++i)
        scrClrRow(ED_ROW1 + i);
}

/* Update editor screen from specified row */
edUpd(row, line) int row, line; {
    int i;

    for (i = row; i < ED_ROWS; ++i) {
        scrClrRow(ED_ROW1 + i);
        if (line < lp_tot) {
#ifdef SHL
          if (ed_shl >= 0)
            ed_shl = shl_highlight(lp_arr[line],
                                   lp_arr[line] + strlen(lp_arr[line]),
                                   lp_arr[line], 1);
          else
#endif
            putstr(lp_arr[line]);
            line++;
        }
    }
}

/* Update editor screen from first row */
edUpdAll() {
    edUpd(0, lp_cur - ed_row);
}

/* Insert Carriage Return at cursor */
insCR() {
    int left_len, right_len, i;
    char *p1, *p2;

    left_len = ed_col;
    right_len = strlen(ln_dat) - ed_col;
    p1 = malloc(left_len + 1);
    p2 = malloc(right_len + 1);
    if (!p1 || !p2) {
        errMsgMemory();
        if (p1)
            free(p1);
        if (p2)
            free(p2);
        return;
    }
    strcpy(p2, ln_dat + ed_col);
    ln_dat[ed_col] = 0;
    strcpy(p1, ln_dat);
    for (i = lp_tot; i > lp_cur; --i)
        lp_arr[i] = lp_arr[i - 1];
    ++lp_tot;
    free(lp_arr[lp_cur]);
    lp_arr[lp_cur++] = p1;
    lp_arr[lp_cur] = p2;
    scrClrEol();
    if (ed_row < ED_ROWS - 1)
        edUpd(++ed_row, lp_cur);
    else
        edUpd(0, lp_cur - ED_ROWS + 1);
    ed_col = 0;
}

/* Delete character on the left of cursor */
delChrLeft() {
    int len_up, len_cur, i;
    char *p;

    len_up = strlen(lp_arr[lp_cur - 1]);
    len_cur = strlen(ln_dat);
    if (len_up + len_cur > MAX_ED_COLS)
        return;
    if (!(p = malloc(len_up + len_cur + 1))) {
        errMsgMemory(); return;
    }
    strcpy(p, lp_arr[lp_cur - 1]);
    strcat(p, ln_dat);
    --lp_tot;
    free(lp_arr[lp_cur]);
    free(lp_arr[lp_cur - 1]);
    for (i = lp_cur; i < lp_tot; ++i)
        lp_arr[i] = lp_arr[i + 1];
    lp_arr[lp_tot] = NULL;
    lp_arr[--lp_cur] = p;
    ed_col = len_up;
    if (ed_row) {
        scrPosCur(ED_ROW1 + --ed_row, ed_col);
        putstr(lp_arr[lp_cur] + ed_col);
        edUpd(ed_row + 1, lp_cur + 1);
    } else
        edUpd(0, lp_cur);
}

/* Delete character on the right of cursor */
delChrRight() {
    int len_dn, len_cur, i;
    char *p;

    len_dn = strlen(lp_arr[lp_cur + 1]);
    len_cur = strlen(ln_dat);
    if (len_dn + len_cur > MAX_ED_COLS)
        return;
    if (!(p = malloc(len_dn + len_cur + 1))) {
        errMsgMemory(); return;
    }
    strcpy(p, ln_dat);
    strcat(p, lp_arr[lp_cur + 1]);
    --lp_tot;
    free(lp_arr[lp_cur]);
    free(lp_arr[lp_cur + 1]);
    for (i = lp_cur + 1; i < lp_tot; ++i)
        lp_arr[i] = lp_arr[i + 1];
    lp_arr[lp_tot] = NULL;
    lp_arr[lp_cur] = p;
    putstr(p + ed_col);
    if (ed_row < ED_ROWS - 1)
        edUpd(ed_row + 1, lp_cur + 1);
}


/* CURSOR MOVEMENT -------------------------------------------------------- */

/* Move cursor one row up */
rowUp() {
    --lp_cur;
    if (ed_row)
        --ed_row;
    else
        edUpd(0, lp_cur);
}

/* Move cursor one row down */
rowDown() {
    ++lp_cur;
    if (ed_row < ED_ROWS - 1)
        ++ed_row;
    else
        edUpd(0, lp_cur - ED_ROWS + 1);
}

/* Move cursor one page up */
pageUp() {
    int first, to;

    first = edGetFirstLineNumber();
    if (first) {
        if ((to = first - ED_ROWS) < 0)
            to = 0;
        lp_cur = to; ed_row = ed_col = 0;
        edUpdAll();
    } else
        toBOF();
}

/* Move cursor one page down */
pageDown() {
    int first, last, to;

    first = edGetFirstLineNumber();
    last = edGetLastLineNumber();
    if (last < lp_tot - 1) {
        to = first + ED_ROWS;
        if (to >= lp_tot)
            to = lp_tot - 1;
        lp_cur = to; ed_row = ed_col = 0;
        edUpdAll();
    } else
        toEOF();
}

/* Move cursor to beginning of file/document */
toBOF() {
    int first;

    first = edGetFirstLineNumber();
    lp_cur = ed_row = ed_col = 0;
    if (first > 0)
        edUpdAll();
}

/* Move cursor to bottom of file/document */
toEOF() {
    int first, last;

    first = edGetFirstLineNumber();
    last = edGetLastLineNumber();
    lp_cur = lp_tot - 1;
    ed_col = MAX_LINES+1;
    if (last < lp_tot - 1) {
        ed_row = ED_ROWS - 1;
        edUpdAll();
    } else
        ed_row = last - first;
}


/* SYSTEM ----------------------------------------------------------------- */

/* Print message on system line */
sysMsg(s) char *s; {
    int i;

    scrClrRow(SY_ROW1);
    scrChrInverted();
    for (i = 0; i < SY_COLS; i++)
        putchar(' ');
    if (s) {
        scrPosCur(SY_ROW1, SY_COLM);
        printf("%s", s);
    }
    scrChrNormal();
    ed_sys_msg = 1;
}

/* Print message on system line and wait for a key press */
sysMsgKey(s) char *s; {
    sysMsg(s);
    keyPressed();
    sysMsg(NULL);
}

/* Print message on system line and wait for confirmation */
int sysMsgConf(s) char *s; {
    int ch;

    sysMsg(s);
    ch = toupper(keyPressed());
    sysMsg(NULL);
    return ch == 'Y' ? 1 : 0;
}

/* Display the filename on system line */
sysPutFileName() {
    int i;

    scrPosCur(SY_ROW1, SY_COLF);
    scrChrInverted();
    for (i = MAX_FNAME - 1; i; --i)
        putchar(' ');
    scrPosCur(SY_ROW1, SY_COLF);
    putstr(fname[0] ? fname : "????????.???");
    scrChrNormal();
}


/* FILE ------------------------------------------------------------------- */

/* Read file to lines */
int fileRead(fn) char *fn; {
    FILE *fp;
    int len, i;
    char ch, *p;

    if (!(fp = fopen(fn, "r"))) {
        fprintf(stderr, "Cannot open %s for reading!", fn);
        return -1;
    }
    for (i = 0; i <= MAX_LINES; ++i) {
        if (!fgets(ln_dat, MAX_ED_COLS + 2, fp))
            break;
        if (i >= MAX_LINES) {
            fclose(fp);
            fprintf(stderr, "Too many lines in file (>%d)!", MAX_LINES);
            return -1;
        }
        len = strlen(ln_dat);
        if (ln_dat[len - 1] == '\n')
            ln_dat[--len] = 0;
        else if (len > MAX_ED_COLS) {
            fclose(fp);
            fprintf(stderr, "Too long lines in file (>%d)!", MAX_ED_COLS);
            return -1;
        }
        if (!(lp_arr[i] = malloc(len + 1))) {
            fclose(fp);
            fprintf(stderr, "Too large file (not enough memory)!");
            return -1;
        }
        strcpy(lp_arr[i], ln_dat);
    }
    fclose(fp);
    lp_tot = i;
    if (!lp_tot) {
        p = malloc(1);
        *p = 0;
        lp_arr[lp_tot++] = p;
    }
    for (i = 0; i < lp_tot; ++i) {
        p = lp_arr[i];
        while ((ch = (*p & 0x7f))) {
            if (ch == '\t') {
                *p = ' '; /* TODO: Replace TAB with n SPC */
            } else if (ch < 0x20) {
                fprintf(stderr, "Unexpected control character!");
                return -1;
            }
            ++p;
        }
    }
    return 0;
}

/* Write lines to file */
int fileWrite(fn) char *fn; {
    FILE *fp;
    int i, err;
    char *p;

    if (!(fp = fopen(fn, "w"))) {
        errMsgOpen();
        return -1;
    }
    for (i = err = 0; i < lp_tot; ++i) {
        p = lp_arr[i];
        while (*p) {
            if (fputc((*p++ & 0x7f), fp) == EOF) {
                ++err;
                break;
            }
        }
        if (!err) {
            if (fputc('\r', fp) == EOF)
                ++err;
            if (fputc('\n', fp) == EOF)
                ++err;
        }
        if (err) {
            fclose(fp);
            errMsgWrite();
            return -1;
        }
    }
    if (fclose(fp) == EOF) {
        errMsgClose();
        return -1;
    }
    return 0;
}


/* OUTPUT ------------------------------------------------------------- */

/* Print ASCII string on screen */
putstr(s) char *s; {
    while (*s)
        putchar(*s++ & 0x7f);
}

/* Print error message and wait for a key press */
errMsg(s) char *s; {
    sysMsgKey(s);
}

errMsgBackup() {
    errMsg("Cannot backup file!");
}

errMsgOpen() {
    errMsg("Cannot open file for writing!");
}

errMsgWrite() {
    errMsg("Cannot write to file!");
}

errMsgClose() {
    errMsg("Cannot close file!");
}

errMsgMemory() {
    errMsg("Not enough memory!");
}

/* SCREEN ------------------------------------------------------------- */

/* Inverted video */
scrChrInverted() {
    printf("\x1b[7m");
}

/* Normal video */
scrChrNormal() {
    printf("\x1b[27m");
}

/* Clear screen and send cursor to upper left corner (0,0). */
scrClr() {
    printf("\x1b[1;1H");
    printf("\x1b[2J");
}

/* Move cursor to row, col */
scrPosCur(row, col) int row, col; {
    printf("\x1b[%d;%dH", row + 1, col + 1);
}

/* Move cursor to row and clear line */
scrClrRow(row) int row; {
    scrPosCur(row, 0);
    scrClrEol();
}

/* Erase from the cursor to the end of the line */
scrClrEol() {
    printf("\x1b[K");
}


/* KEYBOARD --------------------------------------------------------------- */

#asm
; int keyPressed(void);
;   Get character from the keyboard.
        public      keyPressed
keyPressed:
        lhld        1
        lxi         d,6
        dad         d
        lxi         d,keyin2
        push        d
        pchl
keyin2:
        mvi         h,0
        mov         l,a
        ret
#endasm

