/*
    Lean Editor -- a small text editor for programmers.
    Copyright (C) 2017-2022 Lars Lindehaven

    Work based on the Program Text editor (te) v1.08 from 05 Jul 2017.
    Copyright (C) 2015-2016 Miguel Garcia / FloppySoftware

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2, or (at your option) any
    later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 */

/* INCLUDES --------------------------------------------------------------- */

#include "ctype.h"
#include "stdio.h"
#include "le.h"

#ifdef SHL
#include "shl.h"
#endif

/* DEFINITIONS ------------------------------------------------------------ */

#define MAX_FNAME   20
#define MAX_EBLIN   2000
#define MAX_PBLIN   50
#define SY_ROW1     0
#define SY_ROWS     1
#define SY_COLS     (TERM_COLS)
#define SY_COLM     0
#define SY_COLF     1
#define SY_COLI     (SY_COLS - 41)
#define ED_ROW1     SY_ROWS
#define ED_ROWS     (TERM_ROWS - ED_ROW1)
#define ED_COLS     (TERM_COLS)
#define MAX_ED_COLS (TERM_COLS)
#define ED_TABS     (TERM_TABS)
#define MIN_ED_TABS 1
#define MAX_ED_TABS 8


/* GLOBALS */

char            fname[MAX_FNAME];
char            *eb_tmp;
unsigned int    *eb_arr;
unsigned int    *pb_arr;
int             eb_tot;
int             pb_tot;
int             eb_cur;
unsigned int    eb_eds;
int             es_row;
int             es_col;
int             es_tab;
int             es_sys;

#ifdef SHL
int             es_shl = SHL_FAIL;
#endif

int main(argc, argv) int argc, argv[]; {
    int i;
    if (edInit())
        return -1;
    if (argc > 1) {
        if (argc == 3) {
            es_tab = atoi(argv[2]);
            if (es_tab < MIN_ED_TABS || es_tab > MAX_ED_TABS)
                es_tab = TERM_TABS;
#ifdef SHL
            shl_tab_stop(es_tab);
#endif
        }
        if (strlen(argv[1]) > MAX_FNAME - 1) {
            fprintf(stderr, "Filename is too long.");
            return -1;
        } else {
            strcpy(fname, argv[1]);
            if (fileRead(fname))
                return -1;
        }
    } else {
        fprintf(stderr, "Usage: le filename.ext [tab width]");
        return -1;
    }
#ifdef SHL
    es_shl = shl_language(fname);
    if  (es_shl != SHL_FAIL) {
        shl_column(0);
        shl_highlight(eb_arr[0], eb_arr[eb_tot], eb_arr[0], 0);
    }
#endif
    scrClr();
    edLoop();
    scrClr();
    return 0;
}


/* EDITING */

edLoop() {
    int ch;
    sysMsg(NULL);
    sysPutFileName();
    edUpdate(0, eb_cur - es_row);
    while (1) {
        if (es_sys)
            es_sys = 0;
        ch = edGetLine();
        switch (ch) {
            case KEY_RUP :
                toRowUp();
                break;
            case KEY_RDN :
                toRowDown();
                break;
            case KEY_PUP :
                toPageUp();
                break;
            case KEY_PDN :
                toPageDown();
                break;
            case KEY_FBEG :
                toFileBeginning();
                break;
            case KEY_FEND :
                toFileEnd();
                break;
            case KEY_CENTER :
                center();
                break;
            case KEY_SEARCH :
                search();
                break;
            case KEY_LINS :
                insertLine();
                break;
            case KEY_DEL :
                delChrRight();
                break;
            case KEY_RUB :
            case KEY_BS :
                delChrLeft();
                break;
            case KEY_LCUT :
                cutLine();
                break;
            case KEY_PASTE :
                paste();
                break;
            case KEY_LUP :
                mvLineUp();
                break;
            case KEY_LDN :
                mvLineDown();
                break;
            case KEY_FSAVE :
                fileWrite();
                eb_eds = 0;
                edUpdate(0, eb_cur - es_row);
                break;
            case KEY_FQUIT :
                if (!eb_eds)
                    return;
                else if (sysMsgConf("Quit without saving changes? "))
                    return;
                else {
                    sysPutFileName();
                    edUpdate(0, eb_cur - es_row);
                }
                break;
        }
    }
}

edGetLine() {
    int i, ch, len, old_len, run, upd_sys, upd_pos, upd_lin, spc;
    char *buf;
    strcpy(eb_tmp, eb_arr[eb_cur]);
    len = old_len = strlen(eb_tmp);
    run = upd_sys = upd_pos = 1;
    upd_lin = spc = 0;
    if (es_col > len)
        es_col = len;
    while (run) {
        if (upd_lin) {
            upd_lin = 0;
            putString(eb_tmp + es_col);
            if (spc) {
                putchar(' ');
                spc = 0;
            }
        }
        if (upd_sys) {
            upd_sys = 0;
            sysClear();
            sysPutFileName();
            sysPutInfo(eb_eds, es_tab, eb_cur, eb_tot, es_col, len);
        }
        if (upd_pos) {
            upd_pos = 0;
            scrPosCur(ED_ROW1 + es_row, es_col);
        }
        ch = keyPressed();
        switch (ch) {
            case KEY_RUP :
                if (eb_cur)
                    run = 0;
                ++upd_pos;
                break;
            case KEY_RDN :
                if (eb_cur < eb_tot - 1)
                    run = 0;
                ++upd_pos;
                break;
            case KEY_CLT :
                if (es_col) {
                    --es_col;
                    ++upd_sys;
                } else if (eb_cur) {
                    ch = KEY_RUP;
                    es_col = 0x7fff;
                    run = 0;
                }
                ++upd_pos;
                break;
            case KEY_CRT :
                if (es_col < len) {
                    ++es_col;
                    ++upd_sys;
                } else if (eb_cur < eb_tot - 1) {
                    ch = KEY_RDN;
                    es_col = 0;
                    run = 0;
                }
                ++upd_pos;
                break;
            case KEY_WLT :
                if (es_col) {
                    if ((eb_tmp[es_col] & 0x7f) != ' ' &&
                        (eb_tmp[es_col - 1] & 0x7f) == ' ')
                        --es_col;
                    while (es_col && (eb_tmp[es_col] & 0x7f) == ' ')
                        --es_col;
                    while (es_col && (eb_tmp[es_col] & 0x7f) != ' ') {
                        if ((eb_tmp[--es_col] & 0x7f) == ' ') {
                            ++es_col;
                            break;
                        }
                    }
                    ++upd_sys;
                } else if (eb_cur) {
                    ch = KEY_RUP;
                    es_col = 0x7fff;
                    run = 0;
                }
                ++upd_pos;
                break;
            case KEY_WRT :
                if (es_col >= len && eb_cur < eb_tot - 1) {
                    ch = KEY_RDN;
                    es_col = 0;
                    run = 0;
                } else {
                    while (eb_tmp[es_col] && (eb_tmp[es_col] & 0x7f) != ' ')
                        ++es_col;
                    while ((eb_tmp[es_col] & 0x7f) == ' ')
                        ++es_col;
                    ++upd_sys;
                    ++upd_pos;
                }
                break;
            case KEY_LBEG :
                if (es_col) {
                    es_col = 0;
                    ++upd_sys;
                }
                ++upd_pos;
                break;
            case KEY_LEND :
                if (es_col != len) {
                    es_col = len;
                    ++upd_sys;
                }
                ++upd_pos;
                break;
            case KEY_PUP :
            case KEY_FBEG :
                if (eb_cur || es_col)
                    run = 0;
                ++upd_pos;
                break;
            case KEY_PDN :
            case KEY_FEND :
                if (eb_cur < eb_tot - 1 || es_col != len)
                    run = 0;
                ++upd_pos;
                break;
            case KEY_CENTER :
            case KEY_SEARCH :
                run = 0;
                break;
            case KEY_TABW :
                es_tab++;
                if (es_tab > MAX_ED_TABS)
                    es_tab = MIN_ED_TABS;
#ifdef SHL
                shl_tab_stop(es_tab);
#endif
                ++upd_sys;
                ++upd_pos;
                break;
            case KEY_TAB :
                len += tabToSpaces();
                run = 0;
                break;
            case KEY_LINS :
                if (eb_tot < MAX_EBLIN) {
                    run = 0;
                    ++eb_eds;
                }
                break;
            case KEY_DEL :
                /*TODO: Move deleted character to paste buffer */
                if (es_col < len) {
                    strcpy(eb_tmp + es_col, eb_tmp + es_col + 1);
                    --len;
                    ++upd_sys;
                    ++upd_lin;
                    ++eb_eds;
                    ++spc;
                } else if (eb_cur < eb_tot -1) {
                    run = 0;
                }
                ++upd_pos;
                break;
            case KEY_RUB :
            case KEY_BS :
                /*TODO: Move deleted character to paste buffer */
                if (es_col) {
                    strcpy(eb_tmp + es_col - 1, eb_tmp + es_col);
                    --es_col;
                    --len;
                    ++upd_sys;
                    ++upd_lin;
                    ++eb_eds;
                    ++spc;
                    putchar('\b');
                } else if (eb_cur) {
                    run = 0;
                }
                ++upd_pos;
                break;
            case KEY_LCUT :
            case KEY_PASTE :
                run = 0;
                break;
            case KEY_LUP :
                if (eb_cur) {
                    ++eb_eds;
                    run = 0;
                }
                break;
            case KEY_LDN :
                if (eb_cur < eb_tot - 1) {
                    ++eb_eds;
                    run = 0;
                }
                break;
            case KEY_FSAVE :
            case KEY_FQUIT :
                run = 0;
                break;
            default :
                if (len < MAX_ED_COLS && ch >= ' ') {
                    putchar(ch & 0x7f);
                    for (i = len; i > es_col; --i) {
                        eb_tmp[i] = eb_tmp[i - 1];
                    }
                    eb_tmp[es_col++] = ch & 0x7f;
                    eb_tmp[++len] = 0;
                    ++upd_lin;
                    ++upd_sys;
                    ++eb_eds;
                }
                ++upd_pos;
                break;
        }
    }
    if (len == old_len) {
        strcpy(eb_arr[eb_cur], eb_tmp);
    } else if (!(buf = malloc(len + 1))) {
        errMsgMemory();
    } else {
        strcpy(buf, eb_tmp);
        free(eb_arr[eb_cur]);
        eb_arr[eb_cur] = buf;
        edUpdate(0, eb_cur - es_row);
    }
    return ch;
}

int edInit() {
    int i;
    eb_tmp = malloc(MAX_ED_COLS + 2);
    eb_arr = malloc(MAX_EBLIN * 2);
    pb_arr = malloc(MAX_PBLIN * 2);
    if (!eb_tmp || !eb_arr || !pb_arr) {
        fprintf(stderr, "Not enough memory!");
        return -1;
    }
    *eb_tmp = 0;
    for (i = 0; i < MAX_EBLIN; ++i)
        eb_arr[i] = NULL;
    for (i = 0; i < MAX_PBLIN; ++i)
        pb_arr[i] = NULL;
    eb_tot = pb_tot = eb_cur = eb_eds = 0;
    es_row = es_col = 0;
    es_tab = ED_TABS;
    return 0;
}

edGetFirstLineNumber() {
    return eb_cur - es_row;
}

edGetLastLineNumber() {
    int last = edGetFirstLineNumber() + ED_ROWS - 1;
    return last >= eb_tot - 1 ? eb_tot - 1 : last;
}

edClear() {
    int i;
    for (i = 0; i < ED_ROWS; ++i)
        scrClrRow(ED_ROW1 + i);
}

edUpdate(row, line) int row, line; {
    int i, len;
    for (i = row; i < ED_ROWS; ++i) {
        scrClrRow(ED_ROW1 + i);
        if (line < eb_tot) {
#ifdef SHL
          len = strlen(eb_arr[line]);
          if (es_shl >= 0)
            es_shl = shl_highlight(eb_arr[line],
                                   eb_arr[line] + len,
                                   eb_arr[line], 1);
          else
#endif
            putString(eb_arr[line]);
            line++;
        }
    }
}

int tabToSpaces() {
    int len, spc, i, j;
    len = strlen(eb_tmp);
    spc = es_tab - es_col % es_tab;
    if (len + spc <= MAX_ED_COLS) {
        for (i = 0; i < spc; ++i) {
            for (j = len; j > es_col; --j)
                eb_tmp[j] = eb_tmp[j - 1];
            eb_tmp[es_col++] = ' ';
            eb_tmp[++len] = 0;
        }
        ++eb_eds;
    }
    return spc;
}

insertLine() {
    int left_len = es_col;
    int right_len = strlen(eb_tmp) - es_col;
    int i;
    char *p1, *p2;
    if (!(p1 = malloc(left_len + 1))) {
        errMsgMemory(); return;
    }
    if (!(p2 = malloc(right_len + 1))) {
        errMsgMemory(); free(p1); return;
    }
    strcpy(p2, eb_tmp + es_col);
    eb_tmp[es_col] = 0;
    strcpy(p1, eb_tmp);
    for (i = eb_tot; i > eb_cur; --i)
        eb_arr[i] = eb_arr[i - 1];
    ++eb_tot;
    free(eb_arr[eb_cur]);
    eb_arr[eb_cur++] = p1;
    eb_arr[eb_cur] = p2;
    scrClrEol();
    if (es_row < ED_ROWS - 1)
        edUpdate(++es_row, eb_cur);
    else
        edUpdate(0, eb_cur - ED_ROWS + 1);
    es_col = 0;
}

delChrLeft() {
    int len_up = strlen(eb_arr[eb_cur - 1]);
    int len_cur = strlen(eb_tmp);
    int i;
    char *p;
    if (len_up + len_cur > MAX_ED_COLS)
        return;
    if (!(p = malloc(len_up + len_cur + 1))) {
        errMsgMemory(); return;
    }
    strcpy(p, eb_arr[eb_cur - 1]);
    strcat(p, eb_tmp);
    --eb_tot;
    ++eb_eds;
    free(eb_arr[eb_cur]);
    free(eb_arr[eb_cur - 1]);
    for (i = eb_cur; i < eb_tot; ++i)
        eb_arr[i] = eb_arr[i + 1];
    eb_arr[eb_tot] = NULL;
    eb_arr[--eb_cur] = p;
    es_col = len_up;
    if (es_row) {
        scrPosCur(ED_ROW1 + --es_row, es_col);
        putString(eb_arr[eb_cur] + es_col);
        edUpdate(es_row + 1, eb_cur + 1);
    } else
        edUpdate(0, eb_cur);
}

delChrRight() {
    int len_dn = strlen(eb_arr[eb_cur + 1]);
    int len_cur = strlen(eb_tmp);
    int i;
    char *p;
    if (len_dn + len_cur > MAX_ED_COLS)
        return;
    if (!(p = malloc(len_dn + len_cur + 1))) {
        errMsgMemory(); return;
    }
    strcpy(p, eb_tmp);
    strcat(p, eb_arr[eb_cur + 1]);
    --eb_tot;
    ++eb_eds;
    free(eb_arr[eb_cur]);
    free(eb_arr[eb_cur + 1]);
    for (i = eb_cur + 1; i < eb_tot; ++i)
        eb_arr[i] = eb_arr[i + 1];
    eb_arr[eb_tot] = NULL;
    eb_arr[eb_cur] = p;
    putString(p + es_col);
    if (es_row < ED_ROWS - 1)
        edUpdate(es_row + 1, eb_cur + 1);
}

cutLine() {
    int len_cur = strlen(eb_arr[eb_cur]);
    char *p1;
    int i;
    if (pb_tot > MAX_PBLIN - 1 || !(p1 = malloc(len_cur + 1))) {
            errMsgPBFull(); return;
    }
    if (eb_cur < eb_tot) {
        strcpy(p1, eb_arr[eb_cur]);
        pb_arr[pb_tot] = p1;
        pb_tot++;
        for (i = eb_cur; i < eb_tot; ++i)
            eb_arr[i] = eb_arr[i + 1];
        free(eb_arr[eb_tot]);
        eb_arr[eb_tot--] = NULL;
        ++eb_eds;
        edUpdate(0, eb_cur - es_row);
    }
}

paste() {
    int i, j, len_cbcur;
    int cur = eb_cur;
    char *p1;
    for (j = 0; j < pb_tot; j++) {
        len_cbcur = strlen(pb_arr[j]);
        if (!(p1 = malloc(len_cbcur + 1))) {
            errMsgMemory(); return;
        }
        for (i = eb_tot + 1; i > cur; --i)
            eb_arr[i] = eb_arr[i - 1];
        strcpy(p1, pb_arr[j]);
        eb_arr[cur] = p1;
        ++eb_tot;
        ++cur;
        ++eb_eds;
        free(pb_arr[j]);
        pb_arr[j] = NULL;
    }
    pb_tot = 0;
    edUpdate(0, eb_cur - es_row);
}

mvLineUp() {
    int len_up = strlen(eb_arr[eb_cur - 1]);
    int len_cur = strlen(eb_arr[eb_cur]);
    char *p1, *p2;
    if (eb_cur) {
        if (!(p1 = malloc(len_up + 1))) {
            errMsgMemory(); return;
        }
        if (!(p2 = malloc(len_cur + 1))) {
            errMsgMemory(); free(p1); return;
        }
        strcpy(p1, eb_arr[eb_cur - 1]);
        strcpy(p2, eb_arr[eb_cur]);
        free(eb_arr[eb_cur - 1]);
        free(eb_arr[eb_cur]);
        eb_arr[eb_cur - 1] = p2;
        eb_arr[eb_cur] = p1;
        edUpdate(0, eb_cur - es_row);
        toRowUp();
    }
}

mvLineDown() {
    int len_dn = strlen(eb_arr[eb_cur + 1]);
    int len_cur = strlen(eb_arr[eb_cur]);
    char *p1, *p2;
    if (eb_cur < eb_tot - 1) {
        if (!(p1 = malloc(len_dn + 1))) {
            errMsgMemory(); return;
        }
        if (!(p2 = malloc(len_cur + 1))) {
            errMsgMemory(); free(p1); return;
        }
        strcpy(p1, eb_arr[eb_cur + 1]);
        strcpy(p2, eb_arr[eb_cur]);
        free(eb_arr[eb_cur + 1]);
        free(eb_arr[eb_cur]);
        eb_arr[eb_cur + 1] = p2;
        eb_arr[eb_cur] = p1;
        edUpdate(0, eb_cur - es_row);
        toRowDown();
    }
}

/* NAVIGATION */

toRowUp() {
    --eb_cur;
    if (es_row)
        --es_row;
    else
        edUpdate(0, eb_cur);
}

toRowDown() {
    ++eb_cur;
    if (es_row < ED_ROWS - 1)
        ++es_row;
    else
        edUpdate(0, eb_cur - ED_ROWS + 1);
}

toPageUp() {
    int first = edGetFirstLineNumber();
    int to;
    if (first) {
        if ((to = first - ED_ROWS) < 0)
            to = 0;
        eb_cur = to;
        es_row = es_col = 0;
        edUpdate(0, eb_cur - es_row);
    } else
        toFileBeginning();
}

toPageDown() {
    int first = edGetFirstLineNumber();
    int last = edGetLastLineNumber();
    int to;
    if (last < eb_tot - 1) {
        to = first + ED_ROWS;
        if (to >= eb_tot)
            to = eb_tot - 1;
        eb_cur = to;
        es_row = es_col = 0;
        edUpdate(0, eb_cur - es_row);
    } else
        toFileEnd();
}

toFileBeginning() {
    int first = edGetFirstLineNumber();
    eb_cur = es_row = es_col = 0;
    if (first > 0)
        edUpdate(0, eb_cur - es_row);
}

toFileEnd() {
    int first = edGetFirstLineNumber();
    int last = edGetLastLineNumber();
    eb_cur = eb_tot - 1;
    es_col = MAX_EBLIN+1;
    if (last < eb_tot - 1) {
        es_row = ED_ROWS - 1;
        edUpdate(0, eb_cur - es_row);
    } else
        es_row = last - first;
}

center() {
    if (eb_cur > ED_ROWS/2-1 && es_row != ED_ROWS/2-1) {
        es_row = ED_ROWS/2-1;
        edUpdate(0, eb_cur - es_row);
    }
}

nextChar() {
    if (es_col < strlen(eb_arr[eb_cur])) {
        ++es_col;
    } else if (eb_cur < eb_tot){
        eb_cur++;
        es_col = 0;
    }
}

search() {
    static char sstr7[MAX_FNAME];
    static char sstr8[MAX_FNAME];
    static int slen = 0;
    int len;
    char ch;
    do {
        sysPutSearchString(sstr7, slen);
        len = strlen(eb_arr[eb_cur]);
        sysPutInfo(eb_eds, es_tab, eb_cur, eb_tot, es_col, len);
        scrPosCur(ED_ROW1 + es_row, es_col);
        ch = keyPressed();
        if (slen > 0 && (ch == KEY_RUB || ch == KEY_BS))
            slen--;
        else if (slen < MAX_FNAME && ch > 0x1f) {
            sstr7[slen] = ch;
            sstr8[slen] = ch | 0x80;
            slen++;
        }
        if (slen > 0) {
            if (ch == KEY_SEARCH)
                nextChar();
            while (eb_cur < eb_tot &&
                   strncmp(&eb_arr[eb_cur][es_col], sstr7, slen) &&
                   strncmp(&eb_arr[eb_cur][es_col], sstr8, slen)) {
                nextChar();
            }
            if (eb_cur < eb_tot) {
                es_row = 0;
                edUpdate(es_row, eb_cur);
            } else {
                eb_cur = es_row = es_col = 0;
                edUpdate(0, eb_cur - es_row);
            }
        }
    } while (ch != 0x1b && ch != KEY_CLT && ch != KEY_CRT &&
             ch != KEY_RUP && ch != KEY_RDN);
    sysPutFilename();
}

/* SYSTEM LINE */

sysClear() {
    int i;
    scrClrRow(SY_ROW1);
    scrChrInverted();
    for (i = 0; i < SY_COLS; i++)
        putchar(' ');
}
    
sysMsg(s) char *s; {
    int i;
    sysClear();
    if (s) {
        scrPosCur(SY_ROW1, SY_COLM);
        printf("%s", s);
    }
    scrChrNormal();
    es_sys = 1;
}

sysMsgKey(s) char *s; {
    sysMsg(s);
    keyPressed();
    sysPutFileName();
}

int sysMsgConf(s) char *s; {
    int ch;
    sysMsg(s);
    ch = toupper(keyPressed());
    sysMsg(NULL);
    return ch == 'Y' ? 1 : 0;
}

sysPutFileName() {
    sysClear();
    scrPosCur(SY_ROW1, SY_COLF);
    putString("F:");
    putString(fname[0] ? fname : "????????.???");
    scrChrNormal();
}

sysPutSearchString(s, len) char *s; int len; {
    sysClear();
    scrPosCur(SY_ROW1, SY_COLF);
    putString("S:");
    while (len--)
        putchar(*s++ & 0x7f);
    scrChrNormal();
}

sysPutInfo(eds, tab, cur, tot, col, len) int eds, tab, cur, tot, col, len; {
    scrChrInverted();
    scrPosCur(SY_ROW1, SY_COLI);
    printf("P:%02d  ", pb_tot);
    printf("E:%05d  ", eds);
    printf("T:%d  ", tab);
    printf("R:%04d/%04d  ", cur + 1, tot);
    printf("C:%02d/%02d ", col + 1, len);
    scrChrNormal();
}

/* FILE INPUT/OUTPUT */

int fileRead() {
    FILE *fp;
    int line, col;
    char ch, *p;
    if (!(fp = fopen(fname, "r"))) {
        fprintf(stderr, "Cannot open %s for reading!", fname);
        return -1;
    }
    for (line = 0; line <= MAX_EBLIN; ++line) {
        ch = fgetc(fp) & 0x7f;
        for (col = 0;
             col < MAX_ED_COLS + 2 && ch != '\n' && ch != 0x1a && !feof(fp);
             col++) {
            eb_tmp[col] = 0;
            if (ch == '\r')
                ;
            else if (ch == '\t') {
                eb_tmp[col] = ' ';
                eb_tmp[col + 1] = 0;
                es_col = col + 1;
                col += tabToSpaces();
                es_col = 0;
            } else if (ch > 0x1f)
                eb_tmp[col] = ch;
            else {
                fprintf(stderr, "Unexpected control character in file!");
                return -1;
            }
            ch = fgetc(fp) & 0x7f;
        }
        eb_tmp[col] = 0;
        if (line >= MAX_EBLIN) {
            fclose(fp);
            fprintf(stderr, "Too many lines in file (>%d)!", MAX_EBLIN);
            return -1;
        }
        if (col > MAX_ED_COLS + 1) {
            fclose(fp);
            fprintf(stderr, "Too long lines in file (>%d)!", MAX_ED_COLS);
            return -1;
        }
        if (!(eb_arr[line] = malloc(col))) {
            fclose(fp);
            fprintf(stderr, "Too large file (not enough memory)!");
            return -1;
        }
        strcpy(eb_arr[line], eb_tmp);
        if (ch == 0x1a || feof(fp)) {
            line++;
            break;
        }
    }
    fclose(fp);
    eb_tot = line;
    if (!eb_tot) {
        p = malloc(1);
        *p = 0;
        eb_arr[eb_tot++] = p;
    }
    return 0;
}

int fileWrite() {
    FILE *fp;
    int line;
    char *p;
    if (!(fp = fopen(fname, "w"))) {
        errMsgOpen();
        return -1;
    }
    for (line = 0; line < eb_tot - 1; line++) {
        p = eb_arr[line];
        while (*p) {
            if (fputc((*p++ & 0x7f), fp) == EOF) {
                fclose(fp); errMsgWrite(); return -1;
            }
        }
        if (fputc('\r', fp) == EOF || fputc('\n', fp) == EOF) {
            fclose(fp); errMsgWrite(); return -1;
        }
    }
    if (fclose(fp) == EOF) {
        errMsgClose();
        return -1;
    }
    return 0;
}


/* SCREEN OUTPUT */

putString(s) char *s; {
    while (*s)
        putchar(*s++ & 0x7f);
}

errMsg(s) char *s; {
    edUpdate(0, eb_cur - es_row);
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

errMsgPBFull() {
    errMsg("Paste buffer is full!");
}

/* ANSI SCREEN */

scrChrInverted() {
    printf("\x1b[7m");
}

scrChrNormal() {
    printf("\x1b[27m");
}

scrClr() {
    printf("\x1b[1;1H");
    printf("\x1b[2J");
}

scrPosCur(row, col) int row, col; {
    printf("\x1b[%d;%dH", row + 1, col + 1);
}

scrClrRow(row) int row; {
    scrPosCur(row, 0);
    scrClrEol();
}

scrClrEol() {
    printf("\x1b[K");
}


/* CP/M KEYBOARD INPUT */

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
