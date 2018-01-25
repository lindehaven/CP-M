/*
 *  Syntax Highlighter -- prints highlighted source code
 *  Copyright (C) 2017-2018 Lars Lindehaven
 *
 *  Work based on the Program Kilo editor, v0.1.1.
 *  Copyright (c) 2016, Salvatore Sanfilippo <antirez at gmail dot com>
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
#include "stdio.h"
#include "shl.h"

/* DEFINITIONS ------------------------------------------------------------ */

#define SH_MLC      1
#define SH_MLC_END  2
#define SH_SLC      3
#define SH_SLC_END  4
#define SH_STR      5
#define SH_STR_END  6
#define SH_NUM      7
#define SH_KEYW     8
#define SH_PLAIN    9

#ifdef COLOR        /* Syntax highlighting with colors.     */
#define TE_MLC      "\x1b[0;32m"    /* Green                */
#define TE_SLC      "\x1b[0;32m"    /* Green                */
#define TE_STR      "\x1b[0;36m"    /* Normal Cyan          */
#define TE_NUM      "\x1b[0;31m"    /* Normal Red           */
#define TE_KEYW     "\x1b[1;34m"    /* Bold Blue            */
#define TE_PLAIN    "\x1b[0;35m"    /* Normal Magenta       */
#define TE_RESET    "\x1b[39;49m"   /* Reset colors         */
#else               /* Syntax highlighting with attributes. */
#define TE_MLC      "\x1b[2m"       /* Faint                */
#define TE_SLC      "\x1b[2m"       /* Faint                */
#define TE_STR      "\x1b[3m"       /* Italics              */
#define TE_NUM      "\x1b[4m"       /* Underline            */
#define TE_KEYW     "\x1b[1m"       /* Bold                 */
#define TE_PLAIN    "\x1b[0m"       /* Plain                */
#define TE_RESET    "\x1b[0m"       /* Reset attributes     */
#endif

typedef char bool;
#define FALSE       0
#define TRUE        1

struct shl_type {
    char** file_ext;
    char** keywords;
    char* slc_start;
    char* mlc_start;
    char* mlc_end;
};

/* C keywords */
char* shle_cpp[] = {".C", ".H", NULL};
char* shlk_cpp[] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if", "int",
    "long", "register", "return", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void", "volatile",
    "while",
    "#asm", "#define", "#elif", "#else", "#endasm", "#endif", "#error",
    "#ifdef", "#ifndef", "#if", "#include", "#line", "#pragma", "#undef",
    "__DATE__", "__FILE__", "__LINE__", "__STDC__", "__STDC_VERSION__",
    "__TIME__",
    NULL
};

/* CP/M Plus commands*/
char* shle_sub[] = {".SUB", NULL};
char* shlk_sub[] = {
    "COPYSYS", "DATE", "DEVICE", "DIR", "DIRS", "DUMP", "ED", "ERA", "ERASE",
    "GENCOM", "GET", "HELP", "HEXCOM", "INITDIR", "LIB", "LINK", "MAC",
    "PATCH", "PIP", "PUT", "REN", "RENAME", "RMAC", "SAVE", "SET", "SETDEF",
    "SHOW", "SID", "SUBMIT", "TYPE", "USER", "XREF",
    "copysys", "date", "device", "dir", "dirs", "dump", "ed", "era", "erase",
    "gencom", "get", "help", "hexcom", "initdir", "lib", "link", "mac",
    "patch", "pip", "put", "ren", "rename", "rmac", "save", "set", "setdef",
    "show", "sid", "submit", "type", "user", "xref",
    NULL
};

struct shl_type shl_data[] = {
    { shle_cpp, shlk_cpp, "//", "/*", "*/"},
    { shle_sub, shlk_sub, ";",  "", ""}
};

#define SHL_LANGUAGES (sizeof(shl_data) / sizeof(shl_data[0]))

static int g_language = SHL_FAIL;
static int g_state = SH_PLAIN;
static int g_tab_stop = 8;
static int g_column = 0;
static int g_width = 32767;

void emit_str();
void set_mlc();
void clr_mlc();


/* int shl_language(char *lang_file);
 * Sets the desired language for parsing and highlighting.
 *  @lang_file File name with file extension for the desired language.
 *  Returns SHL_DONE if language is supported or SHL_FAIL if language is
 *  not supported.
 */
int shl_language(lang_file)
char *lang_file;
{
    struct shl_type *shl_p;
    int lang_ix, ext_ix, ext_len;
    char* str_p = NULL;

    g_state = SH_PLAIN;
    for (lang_ix = 0; lang_ix < SHL_LANGUAGES; lang_ix++) {
        shl_p = &shl_data[lang_ix];
        ext_ix = 0;
        while (shl_p->file_ext[ext_ix]) {
            str_p = strstr(lang_file, shl_p->file_ext[ext_ix]);
            if (str_p != NULL) {
                ext_len = strlen(shl_p->file_ext[ext_ix]);
                if (shl_p->file_ext[ext_ix][0] == '.' &&
                    str_p[ext_len] == '\0') {
                    g_language = lang_ix;
                    return SHL_DONE;
                }
            }
            ext_ix++;
        }
    }
    g_language = SHL_FAIL;
    return g_language;
}

/* int shl_tab_stop(int tab_stop);
 * Sets the desired tab stop for emitting highlighted strings.
 * @tab_stop Desired tab stop (tab width) in the range {1..8}. Default 8.
 * Returns the set tab stop.
 */
int shl_tab_stop(tab_stop)
int tab_stop;
{
    if (tab_stop >= 1 && tab_stop <= 8)
        g_tab_stop = tab_stop;
    return g_tab_stop;
}

/* int shl_width(int width);
 * Sets the desired screen width for emitting highlighted strings.
 * @width Desired width in the range {1..32767}. Default 32767.
 * Returns the set width.
 */
int shl_width(width)
int width;
{
    if (width >= 1 && width <= 32767)
        g_width = width;
    return g_width;
}

/* int shl_column(int column);
 * Sets the desired column for emitting highlighted strings.
 * @column Desired column in the range {0..32767}. Default 32767.
 * Returns the set column.
 */
int shl_column(column)
int column;
{
    if (column >= 0 && column <= 32767)
        g_column = column;
    return g_column;
}

/* int shl_highlight(char *buf_b, char *buf_e, char *buf_p, int rows);
 * Parses buffer and prints highlighted string on screen.
 *  @buf_b Points to beginning of buffer.
 *  @buf_e Points to end of buffer.
 *  @buf_p Points to part of buffer where the printing shall start.
 *  @rows  Maximum number of rows to print.
 *  Returns number of parsed characters or SHL_FAIL if failed.
 */
int shl_highlight(buf_b, buf_e, buf_p, rows)
char *buf_b, *buf_e, *buf_p; int rows;
{
    int parsed = 0, len = 0, cur_row = 0;
    char *buf_s, *tmp_w, *tmp_b = buf_b;

    if (g_language == SHL_FAIL)
        return SHL_FAIL;
    else {
        buf_s = buf_b;
        while (buf_b < buf_e) {
            if (is_beg_mlc(buf_b) && g_state != SH_STR) {
                g_state = SH_MLC;
                while (buf_b < buf_e && !is_end_mlc(buf_b))
                    set_mlc(buf_b++);
                len = is_end_mlc(buf_b);
                if (len)
                    g_state = SH_MLC_END;
                while (buf_b < buf_e && len--)
                    set_mlc(buf_b++);
            }
            else if (len = is_end_mlc(buf_b) && g_state != SH_STR) {
                if (g_state == SH_MLC)
                    g_state = SH_MLC_END;
                while (buf_b < buf_e && len--)
                    buf_b++;
                tmp_w = buf_b;
                while (tmp_w < buf_e && !is_set_mlc(tmp_w))
                    clr_mlc(tmp_w++);
            }
            else if (is_slc(buf_b) && g_state != SH_STR) {
                g_state = SH_SLC;
                while (buf_b < buf_e && !is_eol(buf_b))
                    buf_b++;
                g_state = SH_SLC_END;
            }
            else if (is_str(buf_b) && g_state != SH_SLC
                     && g_state != SH_MLC) {
                g_state = SH_STR;
                while (++buf_b <= buf_e && !is_eol(buf_b)
                       && !is_str(buf_b))
                    ;
                while (buf_b < buf_e && is_str(buf_b))
                    buf_b++;
                g_state = SH_STR_END;
            }
            else if (is_nums(buf_b) && g_state != SH_SLC
                     && g_state != SH_MLC && g_state != SH_STR) {
                g_state = SH_NUM;
                while (buf_b < buf_e && is_numc(buf_b))
                    buf_b++;
            }
            else if ((len = is_keyw(buf_b)) && g_state != SH_SLC
                     && g_state != SH_MLC && g_state != SH_STR) {
                g_state = SH_KEYW;
                buf_b += len;
            }
            else if (is_eol(buf_b)) {
                cur_row++;
                buf_b++;
                if (g_state == SH_SLC || g_state == SH_STR_END)
                    g_state = SH_PLAIN;
                else if (g_state == SH_MLC) {
                    while (buf_b < buf_e && !is_end_mlc(buf_b))
                        set_mlc(buf_b++);
                    len = is_end_mlc(buf_b);
                    if (len)
                        g_state = SH_MLC;
                    while (buf_b < buf_e && len--)
                        set_mlc(buf_b++);
                }
            }
            else if (g_state == SH_MLC_END) {
                g_state = SH_PLAIN;
            }
            else {
                g_state = SH_PLAIN;
                if (buf_b < buf_e)
                    buf_b++;
            }
            if (buf_b > buf_p && cur_row < rows) {
                switch (g_state) {
                    case SH_MLC :
                    case SH_MLC_END :
                        emit_str(TE_MLC, buf_s, buf_b, buf_e);
                        break;
                    case SH_SLC :
                    case SH_SLC_END :
                        emit_str(TE_SLC, buf_s, buf_b, buf_e);
                        break;
                    case SH_STR :
                    case SH_STR_END :
                        emit_str(TE_STR, buf_s, buf_b, buf_e);
                        break;
                    case SH_NUM :
                        emit_str(TE_NUM, buf_s, buf_b, buf_e);
                        break;
                    case SH_KEYW :
                        emit_str(TE_KEYW, buf_s, buf_b, buf_e);
                        break;
                    case SH_PLAIN :
                        emit_str(TE_PLAIN, buf_s, buf_b, buf_e);
                        break;
                    default:
                        printf("%s", TE_RESET);
                        fprintf(stderr, "\nInternal error!\n");
                        return SHL_FAIL;
                        break;
                }
            }
            parsed += buf_b - buf_s;
            buf_s = buf_b;
        }
        printf("%s", TE_RESET);
        return parsed;
    }
}

/* Prints the highlighting marker and then a string.
 *  @marker Points to the highlighting marker for the terminal.
 *  @buf_s Points to the beginning of the string to print.
 *  @buf_b Points to the end of the string to print.
 *  @buf_e Points to end of buffer.
 */
void emit_str(marker, buf_s, buf_b, buf_e)
char *marker, *buf_s, *buf_b, *buf_e;
{
    char *buf_c, ch;

    printf("%s", marker);
    buf_c = buf_s;
    while (buf_c < buf_b && buf_c < buf_e && g_column < g_width) {
        ch = *buf_c & 0x7f;
        if (ch == '\r' || ch == '\n') {
            putchar(ch);
            g_column = 0;
        } else if (ch == '\t') {
            do
                putchar(' ');
            while (++g_column % g_tab_stop && g_column < g_width);
        } else if (ch > 0 && ch < 0x7f) {
            putchar(ch);
            ++g_column;
        }
        buf_c++;
    }
}

/* Set as multi-line comment */
void set_mlc(str) char *str; {
    *str |= 0x80;
}

/* Clear multi-line comment */
void clr_mlc(str) char *str; {
    *str &= 0x7f;
}

/* Returns 1 if strings are equal or 0 if unequal. */
int is_equ_str(str1, str2, len) char *str1, *str2; int len; {
    char ch1, ch2;
    int i;
    
    for (i = 0; i < len; i++) {
        ch1 = str1[i] & 0x7f;
        ch2 = str2[i] & 0x7f;
        if (ch1 != ch2) break;
    }

    return (i == len);
}

/* Returns length of multi-line comment beginning or 0 if no match. */
int is_beg_mlc(str) char *str; {
    char *look_p;
    int len = 0;

    look_p = shl_data[g_language].mlc_start;
    len = strlen(look_p);
    if (is_equ_str(str, look_p, len))
        return len;
    else
        return 0;
}

/* Returns 1 if multi-line comment or 0 if no match. */
int is_set_mlc(str) char *str; {
    return str[0] & 0x80 ? 1 : 0;
}

/* Returns length of multi-line comment end or 0 if no match. */
int is_end_mlc(str) char *str; {
    char *look_p;
    int len = 0;

    look_p = shl_data[g_language].mlc_end;
    len = strlen(look_p);
    if (is_equ_str(str, look_p, len))
        return len;
    else
        return 0;
}

/* Returns length of single line comment start or 0 if no match */
int is_slc(str) char *str; {
    char *look_p;
    int len = 0;

    look_p = shl_data[g_language].slc_start;
    len = strlen(look_p);
    if (is_equ_str(str, look_p, len))
        return len;
    else
        return 0;
}

/* Returns length of keyword or 0 if no match */
int is_keyw(str) char *str; {
    char **look_p;
    int i, len = 0;

    look_p = shl_data[g_language].keywords;
    for (i = 0; look_p[i]; i++) {
        len = strlen(look_p[i]);
        if (is_equ_str(str, look_p[i], len)
            && is_sep(str[-1]) && is_sep(str[len]))
            return len;
    }
    return 0;
}

/* Returns 1 if string */
int is_str(str) char *str; {
    char ch = str[0] & 0x7f;
    char chp = str[-1] & 0x7f;
    return (ch == '\"' && chp != '\\');
}

/* Returns 1 if numeric start */
int is_nums(str) char *str; {
    char ch = str[0] & 0x7f;
    char chp = str[-1] & 0x7f;
    char chn = str[1] & 0x7f;
    return (isdigit(ch) && is_sep(chp) || ch == '.' && isdigit(chn));
}

/* Returns 1 if numeric continues */
int is_numc(str) char *str; {
    char ch = str[0] & 0x7f;
    return (isdigit(ch) || ch == '.'
            || ch == 'X' || (ch >= 'A' && ch <= 'F')
            || ch == 'x' || (ch >= 'a' && ch <= 'f'));
}

/* Returns 1 if separator character */
int is_sep(ch) char ch; {
    ch &= 0x7f;
    return (isspace(ch) || ch == '\0'
            || strchr(",.()+-/*=~%<>[]:;", ch) != NULL);
}

/* Returns 1 if end of line */
int is_eol(str) char *str; {
    char ch = str[0] & 0x7f;
    return (ch == '\r' || ch == '\n' || ch == '\0');
}

