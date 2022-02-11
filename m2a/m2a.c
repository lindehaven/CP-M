/*  m2a -- convert markdown to ANSI screen codes */

#include "ctype.h"
#include "stdio.h"

/* Send ANSI screen codes */
int reset()        { printf("\x1b[0m");  }
int bold()         { printf("\x1b[1m");  }
int faint()        { printf("\x1b[2m");  }
int normal()       { printf("\x1b[22m"); }
int italics()      { printf("\x1b[3m");  }
int no_italics()   { printf("\x1b[23m"); }
int single_under() { printf("\x1b[4m");  }
int double_under() { printf("\x1b[21m"); }
int no_under()     { printf("x1b[24m");  }
int inverse()      { printf("\x1b[7m");  }
int no_inverse()   { printf("\x1b[27m"); }

/* 0 = no emphasis, 1 = italics, 2 = bold, 3 = italics and bold */
int is_emphasis = 0;

/* 0 = not inline code, 1 = inline code */
int is_inline = 0;

/* 0 = not block, 1 = block */
int is_block = 0;

/* Returns 1 if heading was converted */
int heading(string) char *string; {
    int level = 0;
    if (strstr(string, "#") == string) {
        while (*string++ == '#')
            ++level;
        inverse();
        switch (level) {
            case 1 : single_under();
                     bold();
                     break;
            case 2 : double_under();
                     bold();
                     break;
            case 3 : single_under();
                     normal();
                     break;
            case 4 : double_under();
                     normal();
                     break;
            case 5 : single_under();
                     faint();
                     break;
            default: double_under();
                     faint();
        }
        printf("%s\n", string-1);
        reset();
        return (1);
    } else {
        return (0);
    }
}

/* Returns 1 if quote was converted */
int quote(string) char *string; {
    int level = 0;
    int m;
    if (strstr(string, ">") == string) {
        while (*string++ == '>')
            ++level;
        reset();
        bold();
        for (m = 0; m < level; m++)
            printf("|");
        normal();
        italics();
        printf("%s\n", string-1);
        no_italics();
        return (1);
    } else {
        return (0);
    }
}

/* Returns 1 if code block was converted */
int block(string) char *string; {
    if (strstr(string, "```") == string ||
        strstr(string, "~~~") == string) {
        if (is_block == 1) {
            is_block = 0;
            normal();
        } else {
            is_block = 1;
            bold();
        }
        return (1);
    } else {
        return (0);
    }
}

/* Returns 1 if unordered list was converted */
int unordered(string) char *string; {
    if (strstr(string, "*") == string ||
        strstr(string, "+") == string ||
        strstr(string, "-") == string) {
        reset();
        bold();
        printf("-");
        normal();
        emphasis(string+1);
        return (1);
    } else {
        return (0);
    }
}

/* Returns 1 if plain text or inline code/emphasis was converted */
int emphasis(string) char *string; {
    int i;
    if (!is_block) {
        for (i = 0; i < strlen(string); i++) {
            if (strstr(&string[i], "`") == &string[i]) {
                if (is_inline) {
                    is_inline = 0;
                    normal();
                } else {
                    is_emphasis = 0;
                    is_inline = 1;
                    bold();
                }
                i += 1;
            } else if (!is_inline) {
                if (strstr(&string[i], "***") == &string[i] ||
                    strstr(&string[i], "___") == &string[i]) {
                    if (is_emphasis == 3) {
                        is_emphasis = 0;
                        no_italics();
                        normal();
                    } else {
                        is_emphasis = 3;
                        italics();
                        bold();
                    }
                    i += 3;
                } else if (strstr(&string[i], "**") == &string[i] ||
                           strstr(&string[i], "__") == &string[i]) {
                    if (is_emphasis == 2) {
                        is_emphasis = 0;
                        normal();
                    } else {
                        is_emphasis = 2;
                        bold();
                    }
                    i += 2;
                } else if (strstr(&string[i], "*") == &string[i] ||
                           strstr(&string[i], "_") == &string[i]) {
                    if (is_emphasis == 1) {
                        is_emphasis = 0;
                        no_italics();
                    } else {
                        is_emphasis = 1;
                        italics();
                    }
                    i += 1;
                }
            }
            printf("%c", string[i]);
        }
        printf("\n");
        return (1);
    } else {
        printf("  %s\n", string);
        return (0);
    }
}

/* Convert all supported markdown codes */
int convert(string) char *string; {
    if (!is_block && !is_inline && heading(string))
        ;
    else if (!is_block && !is_inline && quote(string))
        ;
    else if (!is_block && !is_inline && unordered(string))
        ;
    else if (block(string))
        ;
    else
        emphasis(string);
    return (1);
}

/* Read lines and convert until end of file */
int main() {
    char line[1024];
    while (1) {
        if (gets(line) != NULL)
            convert(line);
        else
            break;
    }
    reset();
}
