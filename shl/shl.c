/*
 *  Syntax Highlighter -- prints highlighted source code
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
#include "stdio.h"
#include "shl.h"

/* DEFINITIONS ------------------------------------------------------------ */

#define PROG_NAME   "Syntax Highlighter"
#define PROG_AUTH   "Lars Lindehaven"
#define PROG_VERS   "v0.1.1 2017-11-27"
#define PROG_SYST   "CP/M"
#define PROG_TERM   "ANSI Terminal"

#define SH_MLC      1
#define SH_MLC_END  2
#define SH_SLC      3
#define SH_STR      4
#define SH_NUM      5
#define SH_KEYW     6
#define SH_PLAIN    7

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
#define TE_CLEAR    "\x1b[2J"       /* Clear screen         */
#define TE_HIDE     "\x1b[8m"       /* Hide cursor          */
#define TE_SHOW     "\x1b[28m"      /* Show cursor          */

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
    { shle_a, shlk_a, ";",  "/*", "*/"},
    { shle_c, shlk_c, "//", "/*", "*/"}
};

#define SHL_LANGUAGES (sizeof(shl_data) / sizeof(shl_data[0]))

int shl_language = SH_FAIL;
int state = SH_PLAIN;

/* Sets the desired language for highlighting.
 *  @lang_file File extension of the desired language, ex ".c".
 *  Returns 0 if language is supported or -1 if language is not supported.
 */
int shl_set_language(lang_file) char *lang_file; {
    struct shl_type *shl_p;
    int lang_ix, ext_ix, ext_len;
    char* str_p = NULL;

    int state = SH_PLAIN;
    for (lang_ix = 0; lang_ix < SHL_LANGUAGES; lang_ix++) {
        shl_p = &shl_data[lang_ix];
        ext_ix = 0;
        while (shl_p->file_ext[ext_ix]) {
            str_p = strstr(lang_file, shl_p->file_ext[ext_ix]);
            if (str_p != NULL) {
                ext_len = strlen(shl_p->file_ext[ext_ix]);
                if (shl_p->file_ext[ext_ix][0] == '.' && str_p[ext_len] == '\0') {
                    shl_language = ext_ix;
                    return shl_language;
                }
            }
            ext_ix++;
        }
    }
    shl_language = SH_FAIL;
    return shl_language;
}

/* Prints highlighted text on screen.
 *  @buf_b Points to beginning of buffer.
 *  @buf_e Points to end of buffer.
 *  @buf_p Points to part of buffer where the printing shall start.
 *  @rows Maximum number of rows to print.
 *  Returns 0 when done or -1 if failed or language has not been set.
 */
int shl_highlight(buf_b, buf_e, buf_p, rows) char *buf_b, *buf_e, *buf_p; int rows; {
    int cur_row = 0;
    char *buf_s;

    if (shl_language == SH_FAIL)
        return SH_FAIL;
    else {
        buf_s = buf_b;
        while (buf_b <= buf_e && cur_row < rows) {

            if (state == SH_MLC) {
                while (++buf_b <= buf_e && !is_mlce(buf_b))
                    ;
                if (is_mlce(buf_b))
                    state = SH_MLC_END;
                while (++buf_b <= buf_e && is_mlce(buf_b))
                    ;
            }
            else if (is_mlcb(buf_b) && state != SH_STR) {
                state = SH_MLC;
                while (++buf_b <= buf_e && !is_mlce(buf_b))
                    ;
                if (is_mlce(buf_b))
                    state = SH_MLC_END;
                while (++buf_b <= buf_e && is_mlce(buf_b))
                    ;
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
            }
            else if (is_num(buf_b) && state != SH_SLC && state != SH_MLC
                     && state != SH_STR) {
                state = SH_NUM;
                while (buf_b <= buf_e && is_num(buf_b))
                    buf_b++;
            }
            else if (is_keyw(buf_b) && state != SH_SLC && state != SH_MLC
                     && state != SH_STR) {
                state = SH_KEYW;
                buf_b++;
            }
            else if (is_eol(buf_b) && state == SH_SLC) {
                state = SH_PLAIN;
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
                        shl_print(TE_MLC, buf_s, buf_b);
                        break;
                    case SH_MLC_END :
                        shl_print(TE_MLC, buf_s, buf_b);
                        break;
                    case SH_SLC :
                        shl_print(TE_SLC, buf_s, buf_b);
                        break;
                    case SH_STR :
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
                        printf("\nInternal error! Unknown state\n");
                        return SH_FAIL;
                        break;
                }
            }
            buf_s = buf_b;
        }
        printf("%s", TE_RESET);
        return SH_DONE;
    }
}

/* Prints the highlighting marker and then a string.
 *  @marker Points to the highlighting marker for the terminal.
 *  @buf_s Points to the beginning of the string to print.
 *  @buf_b Points to the end of the string to print.
 *  Returns 0 when done.
 */
int shl_print(marker, buf_s, buf_b) char *marker, *buf_s, *buf_b; {
    char *buf_c;

    printf("%s", marker);
    buf_c = buf_s;
    while (buf_c < buf_b) {
        if (*buf_c)
            putchar(*buf_c);
        buf_c++;
    }
    return SH_DONE;
}

/* Returns 1 if beginning of multi-line comment */
int is_mlcb(str) char *str; {
    return (str[-1] != '\\' && (str[0] == '/' && str[1] == '*'));
}

/* Returns 1 if end of multi-line comment */
int is_mlce(str) char *str; {
    return (str[-2] != '\\' && (str[-1] == '*' && str[0] == '/'));
}

/* Returns 1 if single line comment */
int is_slc(str) char *str; {
    return  (str[-1] != '\\' && (str[0] == '/' && str[1] == '/'));
}

/* Returns 1 if string */
int is_str(str) char *str; {
    return (str[-1] != '\\' && str[0] == '\"');
}

/* Returns 1 if numeric */
int is_num(str) char *str; {
    return (isdigit(str[0]));
}

/* Returns 1 if keyword */
int is_keyw(str) char *str; {
    return FALSE;
}

/* Returns 1 if end of line */
int is_eol(str) char *str; {
    return (str[0] == '\r' || str[0] == '\n' || str[0] == '\0');
}
