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
#define PROG_VERS   "v0.1.4 2017-12-06"
#define PROG_SYST   "CP/M"
#define PROG_TERM   "ANSI Terminal"

#define fputc       putc

#define MAX_FNAME   22
#define MAX_LINES   2000

#define SY_ROW1     0
#define SY_COL1     0
#define SY_COLS     (TERM_COLS)
#define SY_CTAB     (SY_COLS - 25)
#define SY_CROW     (SY_COLS - 20)
#define SY_CCOL     (SY_COLS - 7)

#define ED_ROW1     1
#define ED_ROWS     ((TERM_ROWS) - ED_ROW1)
#define ED_COLS     (TERM_COLS)
#define ED_COL_MAX  ((TERM_COLS) - 1)
#define ED_TABS     (TERM_TABS)
#define ED_TAB_MAX  8

#define FILE_QUIT   128

#define ONSCR_TAB   144

#define QUICK_LBEG  160
#define QUICK_LEND  161
#define QUICK_FBEG  162
#define QUICK_FEND  163


/* GLOBALS ---------------------------------------------------------------- */

/* File name */
char            fname[MAX_FNAME];

/* Lines */
unsigned int    *lp_arr;
int             lp_tot;
int             lp_cur;
char            *ln_dat;
char            *ln_clp;
int             sysln;

/* Editor screen */
int             ed_row;
int             ed_col;
int             ed_tab = ED_TABS;

/* Syntax highlighting */
#ifdef SHL
int             ed_shl = SHL_FAIL;
#endif


/* PROGRAM ---------------------------------------------------------------- */

int main(argc, argv) int argc, argv[]; {
    int i;

    ln_dat = malloc(ED_COL_MAX + 2);
    ln_clp = malloc(ED_COL_MAX + 1);
    lp_arr = malloc(MAX_LINES * 2);
    if (!ln_dat || !ln_clp || !lp_arr) {
        fprintf(stderr, "Not enough memory!");
        return -1;
    }
    *ln_clp = 0;
    for (i = 0; i < MAX_LINES; ++i)
        lp_arr[i] = NULL;
    if (argc == 1) {
        scrClr();
        sysMsg(NULL);
        fileNew();
    } else if (argc == 2) {
        if (strlen(argv[1]) >= MAX_FNAME) {
            fprintf(stderr, "Filename too long!");
            return -1;
        } else {
            scrClr();
            sysMsg(NULL);
            strcpy(fname, argv[1]);
            if (fileRead(fname))
                fileNew();
        }
    } else if (argc > 2) {
        fprintf(stderr, "Usage: le [filename]");
        return -1;
    }
#ifdef SHL
    ed_shl = shl_set_language(fname);
    if  (ed_shl != SHL_FAIL)
        shl_highlight(lp_arr[0], lp_arr[lp_tot], lp_arr[lp_tot], 0);
#endif
    edLoop();
    scrClr();
    return 0;
}


/* EDITING ---------------------------------------------------------------- */

/* Main editor loop */
edLoop() {
    int run, ch;

    run = sysln = 1;
    sysPutFileName();
    edUpdAll();
    while (run) {
        if (sysln)
            sysln = 0;
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
            case QUICK_FBEG :
                toBOF();
                break;
            case QUICK_FEND :
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
            case KEY_FILE :
                if (fcGetCommand())
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
    lp_cur = lp_tot = ed_row = ed_col = 0;
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
            scrPosCur(SY_ROW1, SY_CTAB);
            printf("T:%d", ed_tab);
            scrPosCur(SY_ROW1, SY_CROW);
            printf("R:%04d/%04d", lp_cur + 1, lp_tot);
            scrPosCur(SY_ROW1, SY_CCOL);
            printf("C:%02d/%02d", ed_col + 1, len);
            scrChrNormal();
        }
        if (upd_pos) {
            upd_pos = 0;
            scrPosCur(ED_ROW1 + ed_row, ed_col);
        }
        ch = getTranslatedKey();
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
                    if (ln_dat[ed_col] != ' ' && ln_dat[ed_col - 1] == ' ')
                        --ed_col;
                    while (ed_col && ln_dat[ed_col] == ' ')
                        --ed_col;
                    while (ed_col && ln_dat[ed_col] != ' ') {
                        if (ln_dat[--ed_col] == ' ') {
                            ++ed_col;
                            break;
                        }
                    }
                    ++upd_sys;
                }
                ++upd_pos;
                break;
            case KEY_WRT :
                while (ln_dat[ed_col] && ln_dat[ed_col] != ' ')
                    ++ed_col;
                while (ln_dat[ed_col] == ' ')
                    ++ed_col;
                ++upd_sys;
                ++upd_pos;
                break;
            case KEY_PUP :
            case QUICK_FBEG :
                if (lp_cur || ed_col)
                    run = 0;
                ++upd_pos;
                break;
            case KEY_PDN :
            case QUICK_FEND :
                if (lp_cur < lp_tot - 1 || ed_col != len)
                    run = 0;
                ++upd_pos;
                break;
            case KEY_TAB :
                t = ed_tab - ed_col % ed_tab;
                if (len + t < ED_COL_MAX) {
                    while (t--) {
                        for (i = len; i > ed_col; --i) {
                            putchar(' ');
                            ln_dat[i] = ln_dat[i - 1];
                        }
                        ln_dat[ed_col++] = ' ';
                        ln_dat[++len] = 0;
                    }
                }
                run = 0;
                break;
            case KEY_LNEW :
                if (lp_tot < MAX_LINES)
                    run = 0;
                break;
            case KEY_DEL :
                if (ed_col < len) {
                    strcpy(ln_dat + ed_col, ln_dat + ed_col + 1);
                    --len;
                    ++upd_sys;
                    ++upd_lin;
                    ++spc;
                } else if (lp_cur < lp_tot -1)
                    run = 0;
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
                    putchar('\b');
                } else if (lp_cur)
                    run = 0;
                ++upd_pos;
                break;
            case KEY_FILE :
                run = 0;
                break;
            case KEY_ONSCR :
            case ONSCR_TAB :
                ++upd_sys;
                ++upd_pos;
                break;
            case KEY_QUICK :
                ++upd_sys;
                ++upd_pos;
                break;
            case QUICK_LBEG :
                if (ed_col) {
                    ed_col = 0;
                    ++upd_sys;
                }
                ++upd_pos;
                break;
            case QUICK_LEND :
                if (ed_col != len) {
                    ed_col = len;
                    ++upd_sys;
                }
                ++upd_pos;
                break;
            default :
                if (len < ED_COL_MAX && ch >= ' ') {
                    putchar(ch);
                    for (i = len; i > ed_col; --i) {
                        ln_dat[i] = ln_dat[i - 1];
                    }
                    ln_dat[ed_col++] = ch;
                    ln_dat[++len] = 0;
                    ++upd_lin;
                    ++upd_sys;
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

/* Get keyboard input and translate if necessary */
getTranslatedKey() {
    int ch;

    ch = keyPressed();
    if (ch == KEY_QUICK) {
        ch = keyPressed();
        switch (toupper(ch)) {
            case KEY_QLBEG :
                return QUICK_LBEG;
            case KEY_QLEND :
                return QUICK_LEND;
            case KEY_QFBEG :
                return QUICK_FBEG;
            case KEY_QFEND :
                return QUICK_FEND;
            default :
                return KEY_QUICK;
        }
    } else if (ch == KEY_ONSCR) {
        ch = keyPressed();
        switch (toupper(ch)) {
            case KEY_OTAB :
                ed_tab += 2;
                if (ed_tab > ED_TAB_MAX)
                    ed_tab = 2;
                return ONSCR_TAB;
            default :
                return KEY_ONSCR;
        }       
    }
    return ch;
}

/* Get File command */
fcGetCommand()
{
    int ask, run, stay;

    ask = run = stay = 1;
    while (run) {
        if (ask)
            sysMsg("File command (N/O/S/A/X/Q)? ");
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
    if (len_up + len_cur > ED_COL_MAX)
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
    if (len_dn + len_cur > ED_COL_MAX)
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
        scrPosCur(SY_ROW1, SY_COL1);
        printf("%s", s);
    }
    scrChrNormal();
    sysln = 1;
}

/* Print message on system line and wait for a key press */
sysMsgKey(s) char *s; {
    sysMsg(s);
    keyPressed();
    sysMsg(NULL);
}

/* Print message on system line and wait for confirmation */
sysMsgConf(s) char *s; {
    int ch;

    sysMsg(s);
    ch = toupper(keyPressed());
    sysMsg(NULL);
    return ch == 'Y' ? 1 : 0;
}

/* Display the filename on system line */
sysPutFileName() {
    int i;

    scrPosCur(SY_ROW1, SY_COL1);
    scrChrInverted();
    for (i = MAX_FNAME - 1; i; --i)
        putchar(' ');
    scrPosCur(SY_ROW1, SY_COL1);
    putstr(fname[0] ? fname : "????????.???");
    scrChrNormal();
}

/* Get a filename from system line */
sysGetFileName(fn) char *fn; {
    int ch;

    sysMsg("Filename: ");
    scrChrInverted();
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
    len = strlen(buf);
    while (1) {
        switch ((ch = keyPressed())) {
            case KEY_RUB :
            case KEY_BS :
                if (len) {
                    printf("\b \b");
                    --len;
                }
                break;
            case KEY_LNEW :
            case KEY_FILE :
                buf[len] = 0;
                return ch;
            default :
                if (len < width && ch >= ' ') {
                    buf[len] = ch;
                    len++;
                    putchar(ch);
                }
                break;
        }
    }
}


/* ERRORS ----------------------------------------------------------------- */

/* Print error message and wait for a key press */
errMsg(s) char *s; {
    sysMsgKey(s);
}

/* Cannot open file error */
errMsgOpen() {
    errMsg("Cannot open file!");
}

/* Cannot write to file error */
errMsgWrite() {
    errMsg("Cannot write to file!");
}

/* Cannot close file error */
errMsgClose() {
    errMsg("Cannot close file!");
}

/* Not enough memory error */
errMsgMemory() {
    errMsg("Not enough memory!");
}

/* Line too long error */
errMsgLong() {
    errMsg("Line too long!");
}

/* Too many lines error */
errMsgMany() {
    errMsg("Too many lines!");
}

/* FILE COMMANDS ---------------------------------------------------------- */

/* File command - New file */
fcNew() {
    if (lp_tot > 1 || strlen(lp_arr[0])) {
        if (!sysMsgConf("New file (Y/N)? "))
            return 1;
    }
    fileNew();
    return 0;
}

/* File command - Open file */
fcOpen() {
    char fn[MAX_FNAME];

    if (lp_tot > 1 || strlen(lp_arr[0])) {
        if (!sysMsgConf("Open file (Y/N)? "))
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
        if (ed_shl != SHL_FAIL)
            shl_highlight(lp_arr[0], lp_arr[lp_tot], lp_arr[lp_tot], 0);
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
        if (ed_shl != SHL_FAIL)
            shl_highlight(lp_arr[0], lp_arr[lp_tot], lp_arr[lp_tot], 0);
#endif
        return 0;
    }
    return 1;
}

/* File command - Quit without saving  */
fcQuit() {
    return !sysMsgConf("Quit (Y/N)? ");
}


/* FILE ------------------------------------------------------------------- */

/* Create new file */
fileNew() {
    char *p;

    edFreeLines();
    fname[0] = 0;
    p = malloc(1);
    *p = 0;
    lp_arr[lp_tot++] = p;
}

/* Read file to lines */
fileRead(fn) char *fn; {
    FILE *fp;
    int len, i;
    char ch, *p;

    edFreeLines();
    if (!(fp = fopen(fn, "r"))) {
        errMsgOpen(); return -1;
    }
    for (i = 0; i <= MAX_LINES; ++i) {
        if (!fgets(ln_dat, ED_COL_MAX + 2, fp))
            break;
        if (i >= MAX_LINES) {
            fclose(fp);
            errMsgMany(); return -1;
        }
        len = strlen(ln_dat);
        if (ln_dat[len - 1] == '\n')
            ln_dat[--len] = 0;
        else if (len > ED_COL_MAX) {
            fclose(fp);
            errMsgLong(); return -1;
        }
        if (!(lp_arr[i] = malloc(len + 1))) {
            fclose(fp);
            errMsgMemory(); return -1;
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
        while ((ch = (*p & 0xFF))) {
            if (ch == '\t') {
                *p = ' ';
            } else if (ch < 0x20) {
                sysMsg("Unexpected control character!");
                return -1;
            }
            ++p;
        }
    }
    return 0;
}

/* Backup the previous file with the same name */
fileBackup(fn) char *fn; {
    FILE *fp;
    char *bkp;
    int i;

    if ((fp = fopen(fn, "r"))) {
        fclose(fp);
        bkp = "$$$$$$$$.BAK";
        rename(fn, bkp);
    }
}

/* Write lines to file */
fileWrite(fn) char *fn; {
    FILE *fp;
    int i, err;
    char *p;

    fileBackup(fn);
    if (!(fp = fopen(fn, "w"))) {
        errMsgOpen(); return -1;
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
            errMsgWrite(); return -1;
        }
    }
    if (fclose(fp) == EOF) {
        errMsgClose(); return -1;
    }
    return 0;
}


/* OUTPUT ------------------------------------------------------------- */

/* Print string on screen */
putstr(s) char *s; {
    while (*s)
        putchar(*s++);
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
