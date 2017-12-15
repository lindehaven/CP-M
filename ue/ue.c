/*
    Public Domain 1991 by Anthony Howe.  All rights released.
    Version 1.25 Public Domain 2002 (C) by Terry Loveall.
    Version 1.26 Public Domain 2017 (C) by Lars Lindehaven.

    THIS PROGRAM COMES WITH ABSOLUTELY NO WARRANTY.
    COMPILE AND USE AT YOUR OWN RISK.

    Cursor Control/Command Keys:
    ----------------------------
    left                ^S
    right               ^D
    up                  ^E
    down                ^X
    word left           ^A
    word right          ^F
    goto line begin     ^[
    goto line end       ^]
    pgdown              ^C
    pgup                ^R
    top of file         ^T
    bottom of file      ^B
    del char            ^G
    del prev char       ^H
    del prev char       Backspace
    del rest of line    ^Y
    undo                ^U
    write file          ^W
    look for string     ^L
    quit                ^Q
    ins tab spaces      ^I

*/

#include "ctype.h"
#include "libc.h"

#define BUF (1024*20)
#define MODE 0666
#define TABSZ 8
#define TABM TABSZ-1

#define MAXLINES 25
#define MAXCOLS 80

int COLS = MAXCOLS;
int LINES = 1;
int done;
int row, col;

char str[MAXCOLS];
char prompt[]="Look for: ";
char sstring[MAXCOLS];
char ubuf[BUF];

char buf[BUF];
char *etxt = buf;
char *curp = buf;
char *page, *epage;
char *filename;

typedef struct {
    char ch;
    int pos;
} U_REC;

U_REC* undop = (U_REC*)&ubuf;

typedef struct {
    int X;
    int Y;
} COORD;

COORD outxy;

void left();
void down();
void up();
void right();
void wleft();
void pgdown();
void pgup();
void wright();
void lnbegin();
void lnend();
void top();
void bottom();
void delete();
void bksp();
void delrol();
void file();
void look();
void undo();
void quit();
void nop();

char key[] = { 
    'S' & 0X1F, 'X' & 0X1F, 'E' & 0X1F, 'D' & 0X1F,
    'A' & 0X1F, 'C' & 0X1F, 'R' & 0X1F, 'F' & 0X1F,
    '[' & 0X1F, ']' & 0X1F, 'T' & 0X1F, 'B' & 0X1F,
    'G' & 0X1F, 'H' & 0X1F, 0x7f,       'Y' & 0X1F,
    'U' & 0X1F, 'W' & 0X1F, 'L' & 0X1F, 'Q' & 0X1F,
    '\0'
};

void (*func[])() = {
    left, down, up, right, 
    wleft, pgdown, pgup, wright,
    lnbegin, lnend, top, bottom, 
    delete, bksp, bksp, delrol,
    undo, file, look, quit,
    nop
};

void GetSetTerm(set) int set; {
    if (set)
        putstr("\x1b[0m");
}

void gotoxy(x, y) int x; int y; {
    sprintf(str,"%c[%d;%dH",0x1b,y,x);
    outxy.Y=y; outxy.X=x;
    putstr(str);
}

void clrtoeol() {
    putstr("\x1b[0K");
    gotoxy(outxy.X,outxy.Y);
}

#asm
; int keyPressed(void);
    public  keyPressed
keyPressed:
    lhld    1
    lxi d,6
    dad d
    lxi d,keyin2
    push    d
    pchl
keyin2:
    mvi h,0
    mov l,a
    ret
#endasm

void put1(c) int c; {
    if (c != 0x00 && c != 0x1a) {
        putchar(c & 0x7f);
        outxy.X++;
    }
}

void emitch(c) int c; {
    if(c == '\t'){
        do put1(' '); while(outxy.X & TABM);
    } else
        put1(c);
    if(c == '\n'){outxy.X=0; outxy.Y++;};
}

putstr(s) char *s; {
    while (*s)
        putchar(*s++);
}

char *prevline(p) char *p; {
    while (buf < --p && *p != '\n');
    return (buf < p ? ++p : buf);
}

char *nextline(p) char *p; {
    while (p < etxt && *p++ != '\n');
    return (p < etxt ? p : etxt);
}

char *adjust(p, column) char *p; int column; {
    int i = 0;
    while (p < etxt && *p != '\n' && i < column) {
        i += *p++ == '\t' ? TABSZ-(i&TABM) : 1;
    }
    return (p);
}

void left() {
    if (buf < curp)
        --curp;
} 

void down() {
    curp = adjust(nextline(curp), col);
}

void up() {
    curp = adjust(prevline(prevline(curp)-1), col);
}

void right() {
    if (curp < etxt)
        ++curp;
}

void wleft() {
    while (isspace(*(curp-1)) && buf < curp)
        --curp;
    while (!isspace(*(curp-1)) && buf < curp)
        --curp;
}

void pgdown() {
    page = curp = prevline(epage-1);
    while (0 < row--)
        down();
    epage = etxt;
}

void pgup() {
    int i = MAXLINES;
    while (0 < --i) {
        page = prevline(page-1); 
        up();
    }
}

void wright() {
    while (!isspace(*curp) && curp < etxt)
        ++curp;
    while (isspace(*curp) && curp < etxt)
        ++curp;
}

void lnbegin() {
    curp = prevline(curp);
}

void lnend() {
    curp = nextline(curp);
    left();
}

void top() {
    curp = buf;
}

void bottom() {
    epage = curp = etxt;
}

void cmove(src, dest, cnt) char *src; char *dest; int cnt; {
    if(src > dest){
        while(cnt--) *dest++ = *src++;
    }
    if(src < dest){
        src += cnt;
        dest += cnt;
        while(cnt--) *--dest = *--src;
    }
    etxt += dest-src;
}

void delete() {
    int n = 1;
    if(curp < etxt){
        if(*curp == '\r' && *(curp+1) == '\n'){
            LINES--;
            n = 2;
        }else if (*curp == '\n' && *(curp-1) == '\r'){
            left();
            delete();
        }
        if((char*)&undop[1] < ubuf+BUF){
            undop->ch = *curp;
            undop->pos = -(int)curp;
            undop++;
        }
        cmove(curp+n, curp, etxt-curp);
    }
}

void bksp() {
    if(buf < curp){
        left();
        delete();
    }
}

void delrol() {
    int l=LINES;
    do{ delete();} while(curp < etxt && l == LINES);
}

void undo() {
    if((void*)undop > (void*)ubuf){
        undop--;
        curp = (char*)undop->pos;
        if(undop->pos < 0){
            curp = -(int)curp;
            cmove(curp, curp+1, etxt-curp);
            *curp = undop->ch;
            if(*curp == '\n') LINES++;
        }else{
            if(*curp == '\n') LINES--;
            cmove(curp+1, curp, etxt-curp);
        }
    }
}

void file() {
    int i;
    write(i = creat(filename, MODE), buf, (int)(etxt-buf));
    close(i);
}

void look() {
    char c;
    int i = strlen(sstring);
    gotoxy(1,1);
    clrtoeol();
    putstr(prompt);
    putstr(sstring);
    do {
        c = (char)keyPressed();
        if(c == '\b' || c == 0x7f){
            c = '\b';
            if(!i) continue;
            i--; emitch(c); emitch(' ');
        } else {
            if(i == MAXCOLS) continue;
            sstring[i++] = c;
        }
        if (c != 0x1b) emitch(c);
    } while(c != 0x1b && c != '\n' && c != 0x0c);
    sstring[--i] = 0;
    if(c != 0x1b){
        do{
            right();
        } while(curp < etxt && strncmp(curp,sstring,i));
    }
}

void quit() {
    done = 1;
}

void nop() {
}

void display() {
    int i=0, j=0;
    if (curp < page)
        page = prevline(curp);
    if (epage <= curp) {
        page = curp; 
        i = MAXLINES;
        while (1 < i--) page = prevline(page-1);
    }
    epage = page;
    gotoxy(0,1);
    while (1) {
        if (curp == epage) {
            row = i;
            col = j;
        }
        if (i >= MAXLINES || LINES <= i || etxt <= epage)
            break;
        if (*epage == '\r' || COLS <= j) {
            ++i;
            j = 0;
            clrtoeol();
        }
        if (*epage != '\r') {
            emitch(*epage);
            j += *epage == '\t' ? TABSZ-(j&TABM) : *epage == '\n' ? 0 : 1;
        }
        ++epage;
    }
    i = outxy.Y;
    while(i++ <= MAXLINES){
        clrtoeol();
        gotoxy(1,i);
    }
    gotoxy(col+1, row+1);
}

int main(argc, argv) int argc; char **argv; {
    int i;
    char ch, *p;
    if (argc < 2)
        return (2);
    GetSetTerm(0);
    if (0 < (i = open(filename = *++argv, 0))) {
        etxt += read(i, buf, BUF);
        if (etxt < buf)
            etxt = buf;
        else {
            p = etxt;
            while(p > buf) {
                if(*--p == '\n') LINES++;
                if(*p == 0x00 || *p == 0x1a) --etxt;
            }
        }
        close(i);
    }
    while (!done) {
        display();
        ch = (char)keyPressed(); 
        i = 0; 
        while (key[i] != ch && key[i] != '\0')
            ++i;
        (*func[i])();
        if(key[i] == '\0'){
            if(etxt < buf+BUF-1){
                if(ch == '\r'){
                    cmove(curp, curp+2, etxt-curp);
                    *curp++ = '\r';
                    *curp++ = '\n';
                    LINES++;
                }else{
                    cmove(curp, curp+1, etxt-curp);
                    *curp++ = ch;
                }
                if((char*)&undop[1] < ubuf+BUF){
                    /*undop->ch = curp[-1];*/
                    undop->pos = (int)curp-1;
                    undop++;
                }
            }
        }
    }
    gotoxy(1,MAXLINES+1);
    GetSetTerm(1);
    return (0);
}


