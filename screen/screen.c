#include "stdio.h"

#define SENTINEL "\0x03"

char *all[] = {
    "BLACK",
    "RED",
    "GREEN",
    "YELLOW",
    "BLUE",
    "MAGENTA",
    "CYAN",
    "WHITE",
    SENTINEL
};

int main(argc, argv) int argc; char **argv; {
    int i;
    if (argc<2) {
        printf("Usage : SCREEN foreground-color [background-color]\n");
        printf("Colors: ");
        i=0;
        do {
            printf("%s ", all[i]);
            ++i;
        } while (strcmp(SENTINEL, all[i]) != 0);
    }
    if (argc>1) { /* foreground-color */
        i=0;
        do {
            if (strcmp(argv[1], all[i]) == 0)
                printf("\x1b[3%dm", i);
            ++i;
        } while (strcmp(SENTINEL, all[i]) != 0);
    }
    if (argc>2) { /* background-color */
        i=0;
        do {
            if (strcmp(argv[2], all[i]) == 0)
                printf("\x1b[4%dm", i);
            ++i;
        } while (strcmp(SENTINEL, all[i]) != 0);
    }
}
