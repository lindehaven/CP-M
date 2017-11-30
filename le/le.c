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

#include "ctype.h"
#include "libc.h"
#include "le.h"
#include "shl.h"

/* DEFINITIONS ------------------------------------------------------------ */

#define PROG_NAME   "Lean Editor"
#define PROG_AUTH   "Lars Lindehaven"
#define PROG_VERS   "v0.1.3 2017-11-27"
#define PROG_SYST   "CP/M"
#define PROG_TERM   "ANSI Terminal"

#define fputc       putc

#define MAX_FNAME   22
#define MAX_LINES   2000
#define MAX_CBSIZE  128

#define SY_ROW_1    0
#define SY_ROWS     2
#define SY_FNAME    6
#define SY_TXT      "Row:0000/0000/0000 Col:00/00 Len:00"
#define SY_INF      ((TERM_COLS) - 35)
#define SY_LIN_CUR  ((TERM_COLS) - 31)
#define SY_LIN_NOW  ((TERM_COLS) - 26)
#define SY_LIN_MAX  ((TERM_COLS) - 21)
#define SY_COL_CUR  ((TERM_COLS) - 12)
#define SY_COL_NOW  ((TERM_COLS) -  2)
#define SY_COL_MAX  ((TERM_COLS) -  9)

#ifdef GRAPHICS     /* 8-bit graphics in ruler.     */
#define RU_TAB      'w'
#define RU_CHR      'q'
#else               /* ASCII characters in ruler.   */
#define RU_TAB      '!'
#define RU_CHR      '-'
#endif

#define ED_ROW_1    SY_ROWS
#define ED_ROWS     ((TERM_ROWS) - SY_ROWS)
#define ED_COLS     (TERM_COLS)
#define ED_COL_MAX  ((TERM_COLS) - 1)
#define ED_TABS     (TERM_TABS)

/* GLOBALS ---------------------------------------------------------------- */

/* Lines */
unsigned int    *lp_arr;
int             lp_now;
int             lp_cur;
char            *ln_dat;
char            *ln_clp;
int             sysln;

/* File name */
char            fname[MAX_FNAME];

/* Editor screen */
int             ed_shr;
int             ed_shc;
int             ed_sht = ED_TABS;

/* Syntax highlighting */
#ifdef SHL
int             ed_shl = -1;
#endif

/* Clipboard */
char            *cb_dat;
int             cb_now;
int             cb_set;
int             cb_get;


/* PROGRAM ---------------------------------------------------------------- */

int main(argc, argv) int argc, argv[]; {
    int i;

    scrSetup();
    sysLayout();
    ln_dat = malloc(ED_COL_MAX + 2);
    ln_clp = malloc(ED_COL_MAX + 1);
    cb_dat = malloc(MAX_CBSIZE);
    lp_arr = malloc(MAX_LINES * 2);
    if (!ln_dat || !ln_clp || !cb_dat || !lp_arr) {
        errMsgMem();
        scrReset();
        return 1;
    }
    *ln_clp = 0;
    for (i = 0; i < MAX_LINES; ++i)
        lp_arr[i] = NULL;

    if (argc == 1) {
        fileNew();
    } else if (argc == 2) {
        if (strlen(argv[1]) > MAX_FNAME - 1) {
            errMsg("Filename too long.");
            fileNew();
        } else if (fileRead(argv[1]))
            fileNew();
        else
            strcpy(fname, argv[1]);
#ifdef SHL
        ed_shl = shl_set_language(fname);
#endif
    } else {
        errMsg("Bad command line. Use: le [filename]");
        fileNew();
    }
    edLoop();
    scrClr();
    scrReset();
    return 0;
}


/* EDITING ---------------------------------------------------------------- */

/* Main editor loop */
edLoop() {
    int run, ch;

    cb_now = cb_get = cb_set = 0;
    run = sysln = 1;
    sysPutFileName();
    edUpdAll();
    while (run) {
        if (sysln)
            sysln = 0;
        scrPosCur(SY_ROW_1, SY_LIN_CUR); putint("%04d", lp_cur + 1);
        scrPosCur(SY_ROW_1, SY_LIN_NOW); putint("%04d", lp_now);
        ch = edGetLine();
        switch (ch) {
            case KEY_RUP :
                --lp_cur;
                if(ed_shr)
                    --ed_shr;
                else
                    edUpd(0, lp_cur);
                break;
            case KEY_RDN :
                ++lp_cur;
                if(ed_shr < ED_ROWS - 1)
                    ++ed_shr;
                else
                    edUpd(0, lp_cur - ED_ROWS + 1);
                break;
            case KEY_LNEW :
                insCR();
                break;
            case KEY_RUB :
            case KEY_BS :
                delChrLeft();
                break;
            case KEY_DEL :
                delChrRight();
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
            case KEY_FILE :
                if(fcGetCommand())
                    run = 0;
                else {
                    sysPutFileName();
                    edUpdAll();
                }
                break;
        }
    }
}

/* Free memory for all line pointers */
edFreeLines() {
    int i;

    for (i = 0; i < MAX_LINES; ++i) {
        if (lp_arr[i]) {
            free(lp_arr[i]);
            lp_arr[i] = NULL;
        }
    }
    lp_cur = lp_now = ed_shr = ed_shc = 0;
}

/* Return line number of first line printed on the editor screen */
edGetFirstLineNumber() {
    return lp_cur - ed_shr;
}

/* Return line number of last line printed on the editor screen */
edGetLastLineNumber() {
    int last;

    last = edGetFirstLineNumber() + ED_ROWS - 1;
    return last >= lp_now - 1 ? lp_now - 1 : last;
}

/* Clear the editor screen */
edClr() {
    int i;

    for (i = 0; i < ED_ROWS; ++i)
        scrClrRow(ED_ROW_1 + i);
}

/* Update editor screen from specified row */
edUpd(row, line) int row, line; {
    int i;
#ifdef SHL
    char *buf_b, *buf_e, *buf_p;

    if (ed_shl >= 0) {
        buf_b = lp_arr[0];
        buf_p = lp_arr[line];
        buf_e = lp_arr[line];
        shl_highlight(buf_b, buf_e, buf_p, 0);
        for (i = row; i < ED_ROWS; ++i)
            scrClrRow(ED_ROW_1 + i);
        scrPosCur(ED_ROW_1 + row, 0);
        for (i = row; i < ED_ROWS; ++i) {
            scrClrRow(ED_ROW_1 + i);
            if (line < lp_now) {
                buf_b = lp_arr[line];
                buf_p = lp_arr[line];
                buf_e = lp_arr[line] + strlen(lp_arr[line]);
                line++;
                shl_highlight(buf_b, buf_e, buf_p, 1);
            }
        }
    } else {
        for (i = row; i < ED_ROWS; ++i) {
            scrClrRow(ED_ROW_1 + i);
            if (line < lp_now)
                putstr(lp_arr[line++]);
        }
    }
#else

    for (i = row; i < ED_ROWS; ++i) {
        scrClrRow(ED_ROW_1 + i);
        if (line < lp_now)
            putstr(lp_arr[line++]);
    }
#endif
}

/* Update editor screen from first row */
edUpdAll() {
    edUpd(0, lp_cur - ed_shr);
}

/* Edit current line */
edGetLine() {
    int i, ch, len, run, upd_lin, upd_col, upd_now, upd_cur, spc, old_len;
    char *buf;

    strcpy(ln_dat, lp_arr[lp_cur]);
    len = old_len = strlen(ln_dat);
    run = upd_col = upd_now = upd_cur = 1;
    upd_lin = spc = 0;
    if (ed_shc > len)
        ed_shc = len;
    while (run) {
        if (upd_lin) {
            upd_lin = 0;
            putstr(ln_dat + ed_shc);
            if (spc) {
                putchar(' ');
                spc = 0;
            }
        }
        if (upd_now) {
            upd_now = 0;
            scrPosCur(SY_ROW_1, SY_COL_NOW);
            putint("%02d", len);
        }
        if (upd_col) {
            upd_col = 0;
            scrPosCur(SY_ROW_1, SY_COL_CUR);
            putint("%02d", ed_shc + 1);
        }
        if (upd_cur) {
            upd_cur = 0;
            scrPosCur(ED_ROW_1 + ed_shr, ed_shc);
        }
        if (!(ch = cbGetCh()))
            ch = getTranslatedKey();
        switch (ch) {
            case KEY_CLT :
                if (ed_shc) {
                    --ed_shc;
                    ++upd_col;
                } else if (lp_cur) {
                    ed_shc = ED_COL_MAX + 1; /*TODO: ed_shc = 9999;*/
                    ch = KEY_RUP;
                    run = 0;
                }
                ++upd_cur;
                break;
            case KEY_CRT :
                if (ed_shc < len) {
                    ++ed_shc;
                    ++upd_col;
                } else if (lp_cur < lp_now - 1) {
                    ch = KEY_RDN;
                    ed_shc = run = 0;
                }
                ++upd_cur;
                break;
            case KEY_RUB :
            case KEY_BS :
                if (ed_shc) {
                    strcpy(ln_dat + ed_shc - 1, ln_dat + ed_shc);
                    --ed_shc;
                    --len;
                    ++upd_now;
                    ++upd_lin;
                    ++spc;
                    ++upd_col;
                    putchar('\b');
                } else if (lp_cur)
                    run = 0;
                ++upd_cur;
                break;
            case KEY_DEL :
                if (ed_shc < len) {
                    strcpy(ln_dat + ed_shc, ln_dat + ed_shc + 1);
                    --len;
                    ++upd_now;
                    ++upd_lin;
                    ++spc;
                } else if (lp_cur < lp_now -1)
                    run = 0;
                ++upd_cur;
                break;
            case KEY_CUT :
                scrClrRow(ED_ROW_1 + ed_shr);
                strcpy(ln_clp, ln_dat);
                ln_dat[0] = len = ed_shc = 0;
                ++upd_now;
                ++upd_col;
                ++upd_cur;
                break;
            case KEY_COPY :
                strcpy(ln_clp, ln_dat);
                ++upd_cur;
                break;
            case KEY_PASTE :
                cbStr(ln_clp);
                ++upd_cur;
                break;
            case KEY_PUP :
            case KEY_FBEG :
                if (lp_cur || ed_shc)
                    run = 0;
                ++upd_cur;
                break;
            case KEY_RUP :
                if (lp_cur)
                    run = 0;
                ++upd_cur;
                break;
            case KEY_PDN :
            case KEY_FEND :
                if (lp_cur < lp_now - 1 || ed_shc != len)
                    run = 0;
                ++upd_cur;
                break;
            case KEY_RDN :
                if (lp_cur < lp_now - 1)
                    run = 0;
                ++upd_cur;
                break;
            case KEY_LBEG :
                if (ed_shc) {
                    ed_shc = 0;
                    ++upd_col;
                }
                ++upd_cur;
                break;
            case KEY_LEND :
                if (ed_shc != len) {
                    ed_shc = len;
                    ++upd_col;
                }
                ++upd_cur;
                break;
            case KEY_FILE :
                run = 0;
                break;
            case KEY_LNEW :
                if (lp_now < MAX_LINES)
                    run = 0;
                break;
            case KEY_TAB :
                i = ed_sht - ed_shc % ed_sht;
                while (i--) {
                    if(cbCh(' '))
                        break;
                }
                break;
            case KEY_WLT :
                if (ed_shc) {
                    if (ln_dat[ed_shc] != ' ' && ln_dat[ed_shc - 1] == ' ')
                        --ed_shc;
                    while (ed_shc && ln_dat[ed_shc] == ' ')
                        --ed_shc;
                    while (ed_shc && ln_dat[ed_shc] != ' ') {
                        if (ln_dat[--ed_shc] == ' ') {
                            ++ed_shc;
                            break;
                        }
                    }
                    ++upd_col;
                }
                ++upd_cur;
                break;
            case KEY_WRT :
                while (ln_dat[ed_shc] && ln_dat[ed_shc] != ' ')
                    ++ed_shc;
                while (ln_dat[ed_shc] == ' ')
                    ++ed_shc;
                ++upd_col;
                ++upd_cur;
                break;
            default :
                if (len < ED_COL_MAX && ch >= ' ') {
                    putchar(ch);
                    for (i = len; i > ed_shc; --i) {
                        ln_dat[i] = ln_dat[i - 1];
                    }
                    ln_dat[ed_shc++] = ch;
                    ln_dat[++len] = 0;
                    ++upd_lin;
                    ++upd_now;
                    ++upd_col;
                }
                ++upd_cur;
                break;
        }
    }
    if (len == old_len) {
        /* FIX-ME: Check if data changed before copying ... */
        strcpy(lp_arr[lp_cur], ln_dat);
    } else if (!(buf = malloc(len + 1))) {
        errMsgMem(); /* FIX-ME: Re-print the line with old contents? */
    } else {
        strcpy(buf, ln_dat);
        free(lp_arr[lp_cur]);
        lp_arr[lp_cur] = buf;
    }
    return ch;
}

/* Get keyboard input and translate if necessary */
getTranslatedKey() {
    int ch;

    ch = keyPressed();
    if (ch == KEY_QUICK) {
        ch = keyPressed();
        switch (toupper(ch)) {
            case KEY_QLBEG :
                return KEY_LBEG;
            case KEY_QLEND :
                return KEY_LEND;
            case KEY_QFBEG :
                return KEY_FBEG;
            case KEY_QFEND :
                return KEY_FEND;
        }
    }
    return ch;
}

/* Insert Carriage Return at cursor */
insCR() {
    int left_len, right_len, i;
    char *p1, *p2;

    left_len = ed_shc;
    right_len = strlen(ln_dat) - ed_shc;
    p1 = malloc(left_len + 1);
    p2 = malloc(right_len + 1);
    if (!p1 || !p2) {
        errMsgMem();
        if (p1)
            free(p1);
        if (p2)
            free(p2);
        return;
    }
    strcpy(p2, ln_dat + ed_shc);
    ln_dat[ed_shc] = 0;
    strcpy(p1, ln_dat);
    for (i = lp_now; i > lp_cur; --i)
        lp_arr[i] = lp_arr[i - 1];
    ++lp_now;
    free(lp_arr[lp_cur]);
    lp_arr[lp_cur++] = p1;
    lp_arr[lp_cur] = p2;
    scrClrEol();
    if (ed_shr < ED_ROWS - 1)
        edUpd(++ed_shr, lp_cur);
    else
        edUpd(0, lp_cur - ED_ROWS + 1);
    ed_shc = 0;
}

/* Delete character on the left of cursor */
delChrLeft() {
    int len_up, len_cur, i;
    char *p;

    len_up = strlen(lp_arr[lp_cur - 1]);
    len_cur = strlen(ln_dat);
    if (len_up + len_cur > ED_COL_MAX) {
        /**errMsgLong();**/ return;
    }
    if (!(p = malloc(len_up + len_cur + 1))) {
        errMsgMem(); return;
    }
    strcpy(p, lp_arr[lp_cur - 1]); strcat(p, ln_dat);
    --lp_now;
    free(lp_arr[lp_cur]);
    free(lp_arr[lp_cur - 1]);
    for (i = lp_cur; i < lp_now; ++i)
        lp_arr[i] = lp_arr[i + 1];
    lp_arr[lp_now] = NULL;
    lp_arr[--lp_cur] = p;
    ed_shc = len_up;
    if (ed_shr) {
        scrPosCur(ED_ROW_1 + --ed_shr, ed_shc);
        putstr(lp_arr[lp_cur] + ed_shc);
        edUpd(ed_shr + 1, lp_cur + 1);
    } else
        edUpd(0, lp_cur);
}

/* Delete character on the right of cursor */
delChrRight() {
    int len_dn, len_cur, i;
    char *p;

    len_dn = strlen(lp_arr[lp_cur + 1]);
    len_cur = strlen(ln_dat);
    if (len_dn + len_cur > ED_COL_MAX) {
        /**errMsgLong();**/ return;
    }
    if (!(p = malloc(len_dn + len_cur + 1))) {
        errMsgMem(); return;
    }
    strcpy(p, ln_dat); strcat(p, lp_arr[lp_cur + 1]);
    --lp_now;
    free(lp_arr[lp_cur]);
    free(lp_arr[lp_cur + 1]);
    for (i = lp_cur + 1; i < lp_now; ++i)
        lp_arr[i] = lp_arr[i + 1];
    lp_arr[lp_now] = NULL;
    lp_arr[lp_cur] = p;
    putstr(p + ed_shc);
    if (ed_shr < ED_ROWS - 1)
        edUpd(ed_shr + 1, lp_cur + 1);
}


/* CURSOR MOVEMENT -------------------------------------------------------- */

/* Move cursor to beginning of file/document */
toBOF() {
    int first;

    first = edGetFirstLineNumber();
    lp_cur = ed_shr = ed_shc = 0;
    if(first > 0)
        edUpdAll();
}

/* Move cursor to bottom of file/document */
toEOF() {
    int first, last;

    first = edGetFirstLineNumber();
    last = edGetLastLineNumber();
    lp_cur = lp_now - 1;
    ed_shc = MAX_LINES+1;
    if (last < lp_now - 1) {
        ed_shr = ED_ROWS - 1;
        edUpdAll();
    } else
        ed_shr = last - first;
}

/* Move cursor one page up */
pageUp() {
    int first, to;

    first = edGetFirstLineNumber();
    if( first) {
        if ((to = first - ED_ROWS) < 0)
            to = 0;
        lp_cur = to; ed_shr = ed_shc = 0;
        edUpdAll();
    } else
        toBOF();
}

/* Move cursor one page down */
pageDown() {
    int first, last, to;

    first = edGetFirstLineNumber();
    last = edGetLastLineNumber();
    if (last < lp_now - 1) {
        to = first + ED_ROWS;
        if (to >= lp_now)
            to = lp_now - 1;
        lp_cur = to; ed_shr = ed_shc = 0;
        edUpdAll();
    } else
        toEOF();
}


/* SYSTEM ----------------------------------------------------------------- */

/* Print system layout. */
sysLayout() {
    int i;

    scrClr();
    putstr("File:");
    scrPosCur(SY_ROW_1, SY_INF); putstr(SY_TXT);
    scrPosCur(SY_ROW_1, SY_LIN_MAX); putint("%04d", MAX_LINES);
    scrPosCur(SY_ROW_1, SY_COL_MAX); putint("%02d", TERM_COLS);
    scrPosCur(ED_ROW_1 - 1, 0);
#ifdef GRAPHICS
    putchar(27); putchar('('); putchar('0');
#endif
    for (i = 0; i < TERM_COLS; ++i) {
        if ((i % ed_sht) == 0)
            putchar(RU_TAB);
        else
            putchar(RU_CHR);
    }
#ifdef GRAPHICS
    putchar(27); putchar('('); putchar('B');
#endif
}

/* Print message on system line */
sysMsg(s) char *s; {
    scrClrRow(TERM_ROWS - 1); /* TODO: move to top of screen instead. */
    if (s)
        putstr(s);
    sysln = 1;
}

/* Print message on system line and wait for a key press */
sysMsgKey(s) char *s; {
    sysMsg(s);
    if (s)
        putchar(' ');
    putstr("Press any key to continue: "); keyPressed();
    sysMsg(NULL);
}

/* Print message on system line and wait for confirmation */
sysMsgConf(s) char *s; {
    int ch;

    sysMsg(s);
    if (s)
        putchar(' ');
    putstr("Please confirm Y/N: ");
    ch = toupper(keyPressed());
    sysMsg(NULL);

    return ch == 'Y' ? 1 : 0;
}

/* Display the filename on system line */
sysPutFileName() {
    int i;

    scrPosCur(SY_ROW_1, SY_FNAME);
    for (i = MAX_FNAME - 1; i; --i)
        putchar(' ');
    scrPosCur(SY_ROW_1, SY_FNAME);
    putstr(fname[0] ? fname : "????????.???");
}

/* Get a filename from system line */
sysGetFileName(fn) char *fn; {
    int ch;

    sysMsg("Filename: ");
    ch = sysGetLine(fn, MAX_FNAME - 1);
    sysMsg(NULL);
    if (ch == KEY_LNEW && strlen(fn)) {
        for (ch = 0; ch < strlen(fn); ch++)
            fn[ch] = toupper(fn[ch]);
        return 1;
    }
    return 0;
}

/* Get system line (limited editing) */
sysGetLine(buf, width) char *buf; int width; {
    int len;
    int ch;

    putstr(buf);
    len=strlen(buf);
    while (1) {
        switch ((ch = keyPressed())) {
            case KEY_RUB :
            case KEY_BS :
                if (len) {
                    putchar('\b');
                    putchar(' ');
                    putchar('\b');
                    --len;
                }
                break;
            case KEY_LNEW :
            case KEY_FILE :
                buf[len] = 0;
                return ch;
            default :
                if (len < width && ch >= ' ')
                    buf[len] = ch;
                len++;
                putchar(ch);
                break;
        }
    }
}


/* ERRORS ----------------------------------------------------------------- */

/* Print error message and wait for a key press */
errMsg(s) char *s; {
    sysMsgKey(s);
}

/* No memory error */
errMsgMem() {
    errMsg("Not enough memory.");
}

/* Line too long error */
errMsgLong() {
    errMsg("Line too long.");
}

/* Can't open file error */
errMsgOpen() {
    errMsg("Cannot open.");
}

/* Too many lines error */
errMsgTooMany() {
    errMsg("Too many lines.");
}


/* FILE COMMANDS ---------------------------------------------------------- */

/* Get File command */
fcGetCommand()
{
    int ask, run, stay;

    ask = run = stay = 1;
    while (run) {
        if (ask)
            sysMsg("Enter option: ");
        else
            ++ask;
        switch (toupper(keyPressed())) {
            case KEY_FNEW :
                run = fcNew();
                break;
            case KEY_FOPEN :
                run = fcOpen();
                break;
            case KEY_FSAVE :
                run = fcSave();
                break;
            case KEY_FSAS :
                run = fcSaveAs();
                break;
            case KEY_FEXIT :
                fcSave();
                run = stay = fcQuit();
                break;
            case KEY_FQUIT :
                run = stay = fcQuit();
                break;
            case KEY_FILE :
                run = 0;
                break;
            default :
                ask = 0;
                break;
        }
    }
    edClr();
    sysMsg(NULL);
    return !stay;
}

/* File command - New file */
fcNew() {
    if (lp_now > 1 || strlen(lp_arr[0])) {
        if (!sysMsgConf(NULL))
            return 1;
    }
    fileNew();
    return 0;
}

/* File command - Open file */
fcOpen() {
    char fn[MAX_FNAME];

    if (lp_now > 1 || strlen(lp_arr[0])) {
        if (!sysMsgConf(NULL))
            return 1;
    }
    fn[0] = 0;
    if (sysGetFileName(fn)) {
        if (fileRead(fn))
            fileNew();
        else
            strcpy(fname, fn);
#ifdef SHL
        ed_shl = shl_set_language(fname);
#endif
        return 0;
    }
    return 1;
}

/* File command - Save file */
fcSave() {
    if (!fname[0])
        return fcSaveAs();
    fileWrite(fname);
    return 1;
}

/* File command - Save file as */
fcSaveAs() {
    char fn[MAX_FNAME];

    strcpy(fn, fname);
    if (sysGetFileName(fn)) {
        if (!fileWrite(fn))
            strcpy(fname, fn);
#ifdef SHL
        ed_shl = shl_set_language(fname);
#endif
        return 0;
    }
    return 1;
}

/* File command - Quit without saving  */
fcQuit() {
    return !sysMsgConf(NULL);
}


/* FILE ------------------------------------------------------------------- */

/* Create new file */
fileNew() {
    char *p;

    edFreeLines();
    fname[0] = 0;
    p = malloc(1);
    *p = 0;
    lp_arr[lp_now++] = p;
}

/* Read file to lines */
fileRead(fn) char *fn; {
    FILE *fp;
    int ch, code, len, i, tabs;
    char *p;

    edFreeLines();
    code = 0;
    sysMsg("Reading file.");
    if (!(fp = fopen(fn, "r"))) {
        errMsgOpen();
        return -1;
    }
    for (i = 0; i <= MAX_LINES; ++i) {
        if (!fgets(ln_dat, ED_COL_MAX + 2, fp))
            break;
        if (i >= MAX_LINES) {
            errMsgTooMany();
            ++code;
            break;
        }
        len = strlen(ln_dat);
        if (ln_dat[len - 1] == '\n')
            ln_dat[--len] = 0;
        else if (len > ED_COL_MAX) {
            errMsgLong();
            ++code;
            break;
        }
        if (!(lp_arr[i] = malloc(len + 1))) {
            errMsgMem();
            ++code;
            break;
        }
        strcpy(lp_arr[i], ln_dat);
    }
    fclose(fp);
    if (code)
        return -1;
    lp_now = i;
    if (!lp_now) {
        p = malloc(1);
        *p = 0;
        lp_arr[lp_now++] = p;
    }
    for (i = tabs = 0; i < lp_now; ++i) {
        p = lp_arr[i];
        while ((ch = (*p & 0xFF))) {
            if (ch < ' ') {
                if (ch == '\t') {
                    *p = ' '; ++tabs;
                } else {
                    errMsg("Bad character found.");
                    return -1;
                }
            }
            ++p;
        }
    }
    if (tabs)
        errMsg("Tabs changed to spaces.");
    return 0;
}

/* Backup the previous file with the same name */
fileBackup(fn) char *fn; {
    FILE *fp;
    char *bkp;

    if ((fp = fopen(fn, "r"))) {
        fclose(fp);
        bkp = "te.bkp";
        /*remove(bkp); /* TODO: Remove file #ifdef CC_AZTEC. */
        rename(fn, bkp);
    }
}

/* Write lines to file */
fileWrite(fn) char *fn; {
    FILE *fp;
    int i, err;
    char *p;

    sysMsg("Writing file.");
    fileBackup(fn);
    if (!(fp = fopen(fn, "w"))) {
        errMsgOpen();
        return -1;
    }

    for (i = err = 0; i < lp_now; ++i) {
        p = lp_arr[i];
        /* FIX-ME: We don't have fputs() yet! */
        while (*p) {
            if (fputc(*p++, fp) == EOF) {
                ++err;
                break;
            }
        }
        if (!err) {
            if(fputc('\n', fp) == EOF)
                ++err;
        }
        if (err) {
            fclose(fp);
            /*remove(fn); /* TODO: Remove file */
            errMsg("Can't write.");
            return -1;
        }
    }
    if (fclose(fp) == EOF) {
        /*remove(fn); /* TODO: Remove file */
        errMsg("Can't close.");
        return -1;
    }
    return 0;
}


/* CLIPBOARD ---------------------------------------------------------- */

/* Add a character to clipboard */
cbCh(ch) int ch; {
    if (cb_now < MAX_CBSIZE) {
        ++cb_now;
        if (cb_set == MAX_CBSIZE)
            cb_set = 0;
        cb_dat[cb_set++] = ch;
        return 0;
    }
    return -1;
}

/* Add a string to clipboard */
cbStr(s) char *s; {
    while (*s)
        if (cbCh(*s++))
            return -1;
    return 0;
}

/* Return character from clipboard */
cbGetCh() {
    if (cb_now) {
        --cb_now;
        if (cb_get == MAX_CBSIZE)
            cb_get = 0;
        return cb_dat[cb_get++];
    }
    return 0;
}


/* OUTPUT ------------------------------------------------------------- */

/* Print string on screen */
putstr(s) char *s; {
    while (*s)
        putchar(*s++);
}

/* Print integer on screen */
putint(format, value) char *format; int value; {
    char r[7];

    sprintf(r, format, value);
    putstr(r);
}


/* SCREEN ------------------------------------------------------------- */

/* Setup terminal screen: Used when the editor starts */
scrSetup() {
}

/* Reset terminal screen: Used when the editor exits */
scrReset() {
}

/* Conceal Cursor */
scrConcealCur() {
    putchar(27); putstr("[8m");
}

/* Reveal Cursor */
scrRevealCur() {
    putchar(27); putstr("[28m");
}

/* Clear screen and send cursor to upper left corner (0,0). */
scrClr() {
    putchar(27); putstr("[1;1H");
    putchar(27); putstr("[2J");
}

/* Move cursor to row, col */
scrPosCur(row, col) int row, col; {
    putchar(27); putchar('[');
    putint("%d", row + 1); putchar(';');
    putint("%d", col + 1); putchar('H');
}

/* Move cursor to row and clear line */
scrClrRow(row) int row; {
    scrPosCur(row, 0);
    scrClrEol();
}

/* Erase from the cursor to the end of the line */
scrClrEol() {
    putchar(27); putstr("[K");
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
