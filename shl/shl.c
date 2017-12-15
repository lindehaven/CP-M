/*
 *  Syntax Highlighter -- prints highlighted source code
 *  Copyright (C) 2017 Lars Lindehaven
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
#define SH_STR      4
#define SH_STR_END  5
#define SH_NUM      6
#define SH_KEYW     7
#define SH_PLAIN    8

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

char* shle_a[] = {".ASM", NULL};
char* shlk_a[] = {
    "add", "adi", "lxi", "mov",
    NULL
};

char* shle_c[] = {".C", ".H", ".CPP", ".HPP", ".CC", NULL};
char* shlk_c[] = {
    "switch", "if", "while", "for", "break", "continue", "return",
    "else", "struct", "union", "typedef", "static", "enum", "class",
    "case", "#include", "volatile", "register", "sizeof", "typedef",
    "union", "goto", "const", "auto", "#define", "#if", "#endif",
    "#error", "#ifdef", "#ifndef", "#undef", "int", "long", "double",
    "float", "char", "unsigned", "signed", "void", "bool",
    NULL
};

struct shl_type shl_data[] = {
    { shle_a, shlk_a, ";",  "", ""},
    { shle_c, shlk_c, "//", "/*", "*/"}
};

#define SHL_LANGUAGES (sizeof(shl_data) / sizeof(shl_data[0]))

int shl_language = SHL_FAIL;
int state = SH_PLAIN;

/* int shl_set_language(char *lang_file);
 * Sets the desired language for parsing and highlighting.
 *  @lang_file File name with file extension for the desired language.
 *  Returns SHL_DONE if language is supported or SHL_FAIL if language is
 *  not supported.
 */
int shl_set_language(lang_file)
char *lang_file;
{
    struct shl_type *shl_p;
    int lang_ix, ext_ix, ext_len;
    char* str_p = NULL;

    state = SH_PLAIN;
    for (lang_ix = 0; lang_ix < SHL_LANGUAGES; lang_ix++) {
        shl_p = &shl_data[lang_ix];
        ext_ix = 0;
        while (shl_p->file_ext[ext_ix]) {
            str_p = strstr(lang_file, shl_p->file_ext[ext_ix]);
            if (str_p != NULL) {
                ext_len = strlen(shl_p->file_ext[ext_ix]);
                if (shl_p->file_ext[ext_ix][0] == '.' &&
                    str_p[ext_len] == '\0') {
                    shl_language = lang_ix;
                    return SHL_DONE;
                }
            }
            ext_ix++;
        }
    }
    shl_language = SHL_FAIL;
    return shl_language;
}

/* int shl_highlight(char *buf_b, char *buf_e, char *buf_p, int rows);
 * Parses buffer and prints highlighted string on screen.
 *  @buf_b Points to beginning of buffer.
 *  @buf_e Points to end of buffer.
 *  @buf_p Points to part of buffer where the printing shall start.
 *  @rows  Maximum number of rows to print.
 *  Returns SHL_DONE when done or SHL_FAIL if failed or language has not
 *  been set.
 */
int shl_highlight(buf_b, buf_e, buf_p, rows)
char *buf_b, *buf_e, *buf_p; int rows;
{
    int len, cur_row = 0;
    char *buf_s;

    if (shl_language == SHL_FAIL)
        return SHL_FAIL;
    else {
        buf_s = buf_b;
        while (buf_b <= buf_e && cur_row < rows) {
            if (is_mlcs(buf_b) && state != SH_STR || state == SH_MLC) {
                state = SH_MLC;
                while (++buf_b <= buf_e && !is_mlce(buf_b))
                    set_mlc(buf_b-1);
                if (len = is_mlce(buf_b)) {
                    state = SH_MLC_END;
                    clr_mlc(buf_b);
                    while (++buf_b <= buf_e && len--)
                        clr_mlc(buf_b);
                }
            }
            else if (is_slc(buf_b) && state != SH_STR) {
                state = SH_SLC;
                while (++buf_b <= buf_e && !is_eol(buf_b))
                    ;
            }
            else if (is_str(buf_b) && state != SH_SLC && state != SH_MLC) {
                state = SH_STR;
                while (++buf_b <= buf_e && !is_str(buf_b))
                    ;
                while (++buf_b <= buf_e && is_str(buf_b))
                    ;
                state = SH_STR_END;
            }
            else if (is_nums(buf_b) && state != SH_SLC && state != SH_MLC
                     && state != SH_STR) {
                state = SH_NUM;
                while (buf_b <= buf_e && is_numc(buf_b))
                    buf_b++;
            }
            else if ((len = is_keyw(buf_b)) && state != SH_SLC
                     && state != SH_MLC && state != SH_STR) {
                state = SH_KEYW;
                buf_b += len;
            }
            else if (is_eol(buf_b)) {
                if (state == SH_SLC)
                    state = SH_PLAIN;
                cur_row++;
                buf_b++;
            }
            else if (state == SH_MLC_END) {
                state = SH_PLAIN;
            }
            else {
                state = SH_PLAIN;
                buf_b++;
            }
            if (buf_b > buf_p) {
                switch (state) {
                    case SH_MLC :
                    case SH_MLC_END :
                        shl_print(TE_MLC, buf_s, buf_b);
                        break;
                    case SH_SLC :
                        shl_print(TE_SLC, buf_s, buf_b);
                        break;
                    case SH_STR :
                    case SH_STR_END :
                        shl_print(TE_STR, buf_s, buf_b);
                        break;
                    case SH_NUM :
                        shl_print(TE_NUM, buf_s, buf_b);
                        break;
                    case SH_KEYW :
                        shl_print(TE_KEYW, buf_s, buf_b);
                        break;
                    case SH_PLAIN :
                        shl_print(TE_PLAIN, buf_s, buf_b);
                        break;
                    default:
                        printf("%s", TE_RESET);
                        fprintf(stderr, "\nInternal error! Unknown state\n");
                        return SHL_FAIL;
                        break;
                }
            }
            buf_s = buf_b;
        }
        printf("%s", TE_RESET);
        return SHL_DONE;
    }
}

/* Prints the highlighting marker and then a string.
 *  @marker Points to the highlighting marker for the terminal.
 *  @buf_s Points to the beginning of the string to print.
 *  @buf_b Points to the end of the string to print.
 *  Returns 0 when done.
 */
shl_print(marker, buf_s, buf_b) char *marker, *buf_s, *buf_b; {
    char *buf_c;

    printf("%s", marker);
    buf_c = buf_s;
    while (buf_c < buf_b) {
        if (*buf_c)
            putchar(*buf_c & 0x7f);
        buf_c++;
    }
}

/* Set as multi-line comment */
set_mlc(buf) char *buf; {
    *buf = *buf | 0x80;
}

/* Clear multi-line comment */
clr_mlc(buf) char *buf; {
    *buf = *buf & 0x7f;
}

/* Returns length of multi-line comment start or 0 if no match */
int is_mlcs(str) char *str; {
    char *look_p;
    int len = 0;

    look_p = shl_data[shl_language].mlc_start;
    len = strlen(look_p);
    if (!strncmp(str, look_p, len) && str[-1] != '\\' || (str[0] & 0x80))
        return len;
    else
        return 0;
}

/* Returns length of multi-line comment end or 0 if no match */
int is_mlce(str) char *str; {
    char *look_p;
    int len = 0;

    look_p = shl_data[shl_language].mlc_end;
    len = strlen(look_p);
    if (!strncmp(str, look_p, len) && str[-1] != '\\')
        return len;
    else
        return 0;
}

/* Returns length of single line comment start or 0 if no match */
int is_slc(str) char *str; {
    char *look_p;
    int len = 0;

    look_p = shl_data[shl_language].slc_start;
    len = strlen(look_p);
    if (!strncmp(str, look_p, len) && str[-1] != '\\')
        return len;
    else
        return 0;
}

/* Returns length of keyword or 0 if no match */
int is_keyw(str) char *str; {
    char **look_p;
    int i, len = 0;

    look_p = shl_data[shl_language].keywords;
    for (i = 0; look_p[i]; i++) {
        len = strlen(look_p[i]);
        if (!strncmp(str, look_p[i], len)
            && is_sep(str[-1]) && is_sep(str[len]))
            return len;
    }
    return 0;
}

/* Returns 1 if string */
int is_str(str) char *str; {
    return (str[-1] != '\\' && str[0] == '\"');
}

/* Returns 1 if numeric start */
int is_nums(str) char *str; {
    return (isdigit(str[0]) && is_sep(str[-1])
            || str[0] == '.' && isdigit(str[1]));
}

/* Returns 1 if numeric continues */
int is_numc(str) char *str; {
    char ch = tolower(*str);
    return (isdigit(ch) || ch == '.'
            || ch == 'x' || (ch >= 'a' && ch <= 'f'));
}

/* Returns 1 if separator character */
int is_sep(ch) char ch; {
    return (isspace(ch) || ch == '\0'
            || strchr(",.()+-/*=~%<>[]:;", ch) != NULL);
}

/* Returns 1 if end of line */
int is_eol(str) char *str; {
    return (str[0] == '\r' || str[0] == '\n' || str[0] == '\0');
}

