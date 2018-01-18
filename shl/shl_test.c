/*
 *  Syntax Highlighter -- tests highlighted source code
 *  Copyright (C) 2017-2018 Lars Lindehaven
 *
 */

#include "shl.h"
#include "stdio.h"

int tcno = 0;
int passed = 0;
int failed = 0;

void suite1() {
    char *lang, *mlcs, *mlce, *slc, *str, *code1, *code2;
    char *buf_b, *buf_e, *buf_p;
    int res = 0;

    lang = "FILEIS.???";
    printf("\nTestcase %2d: shl_set_language()", ++tcno);
    if ((res = shl_set_language(lang)) == SHL_FAIL)
    {++passed; printf("\nPassed");} else {++failed; printf("\nFailed");}

    printf("\nTestcase %2d: shl_highlight()", ++tcno);
    if ((res = shl_highlight(buf_b,buf_e,buf_p,10)) != buf_e-buf_b)
    {++passed; printf("\nPassed");} else {
     ++failed; printf("\nFailed : %d != %d\n",res,buf_e-buf_b);}

    lang = "FILEIS.C";
    printf("\nTestcase %2d: shl_set_language()", ++tcno);
    if ((res = shl_set_language(lang)) == SHL_DONE)
    {++passed; printf("\nPassed");} else {++failed; printf("\nFailed");}

    mlcs = "/*";
    printf("\nTestcase %2d: is_mlcs()", ++\tcno);
    if ((res = is_mlcs(mlcs)) == 2)
    {++passed; printf("\nPassed");} else {++failed; printf("\nFailed");}

    mlce = "*/";
    printf("\nTestcase %2d: is_mlce()", ++\tcno);
    if ((res = is_mlce(mlce)) == 2)
    {++passed; printf("\nPassed");} else {++failed; printf("\nFailed");}

    printf("\nTestcase %2d: is_mlce()", ++\tcno);
    if ((res = is_mlce(&mlce[1])) == 0)
    {++passed; printf("\nPassed");} else {++failed; printf("\nFailed");}

    slc = "//";
    printf("\nTestcase %2d: is_slc()", ++\tcno);
    if ((res = is_slc(slc)) == 2)
    {++passed; printf("\nPassed");} else {++failed; printf("\nFailed");}

    str = "\"";
    printf("\nTestcase %2d: is_str()", ++\tcno);
    if ((res = is_str(str)))
    {++passed; printf("\nPassed");} else {++failed; printf("\nFailed");}

    printf("\nTestcase %2d: is_keyw()", ++\tcno);
    if ((res = is_keyw("int four = 4;")) == 3)
    {++passed; printf("\nPassed");} else {++failed; printf("\nFailed");}

    code1 = "\n  main(argc,argv) { // switch else case\n    int intchar=0xaf; //int\n    /*float*/ float pi_float=3.14;\n    printf(\"Hello \\\"world\\\"!\");\n    /* Here starts a multi-line comment";
    buf_b = buf_p = code1;
    buf_e = buf_b + strlen(code1);
    printf("\nTestcase %2d: shl_highlight()", ++tcno);
    if ((res = shl_highlight(buf_b,buf_e,buf_p,6)) == buf_e-buf_b)
    {++passed; printf("\nPassed");} else {
     ++failed; printf("\nFailed : %d != %d\n",res,buf_e-buf_b);}

    code2 = "\n       and the multi-line comment ends here!\n     */";
    buf_b = buf_p = code2;
    buf_e = buf_b + strlen(code2);
    printf("\nTestcase %2d: shl_highlight()", ++tcno);
    if ((res = shl_highlight(buf_b,buf_e,buf_p,3)) == buf_e-buf_b)
    {++passed; printf("\nPassed");} else {
     ++failed; printf("\nFailed : %d != %d\n",res,buf_e-buf_b);}

}

void suite2() {
    char *lang, *slc, *code1, *code2;
    char *buf_b, *buf_e, *buf_p;
    int res = 0;

    lang = "FILEIS.SUB";
    printf("\nTestcase %2d: shl_set_language()", ++tcno);
    if ((res = shl_set_language(lang)) == SHL_DONE)
    {++passed; printf("\nPassed");} else {++failed; printf("\nFailed");}

    code1 = "\n  \"This string does not continue\n  here but \"here\".";
    buf_b = buf_p = code1;
    buf_e = buf_b + strlen(code1);
    printf("\nTestcase %2d: shl_highlight()", ++tcno);
    if ((res = shl_highlight(buf_b,buf_e,buf_p,3)) == buf_e-buf_b)
    {++passed; printf("\nPassed");} else {
     ++failed; printf("\nFailed : %d != %d\n",res,buf_e-buf_b);}

    code2 = "\n  ; TYPE FILE.TXT\n  TYPE FILE.TXT\n  dir *.123";
    buf_b = buf_p = code2;
    buf_e = buf_b + strlen(code2);
    printf("\nTestcase %2d: shl_highlight()", ++tcno);
    if ((res = shl_highlight(buf_b,buf_e,buf_p,4)) == buf_e-buf_b)
    {++passed; printf("\nPassed");} else {
     ++failed; printf("\nFailed : %d != %d\n",res,buf_e-buf_b);}

}

int main(argc, argv) int argc; char* argv[]; {

    printf("Copyright (C) 2017-2018 by Lars Lindehaven.\n");
    printf("Test of modules for Syntax Highlighter.\n");

    suite1();
    suite2();

    if (!failed)
        printf("\nSuccess! %d of %d tests passed.\n", passed, passed+failed);
    else
        printf("\nFailure! %d of %d tests failed.\n", failed, passed+failed);
}
