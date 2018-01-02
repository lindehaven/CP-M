/*
 *  Syntax Higlighter -- tests highlighted source code
 *  Copyright (C) 2017 Lars Lindehaven
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
    printf("\nTestcase %2d: shl_set_language()  ", ++tcno);
    if ((res = shl_set_language(lang)) == SHL_FAIL)
    {++passed; printf("Passed");} else {++failed; printf("Failed");}

    printf("\nTestcase %2d: shl_highlight()     ", ++tcno);
    if ((res = shl_highlight(buf_b,buf_e,buf_p,10)) == SHL_FAIL)
    {++passed; printf("Passed");} else {++failed; printf("Failed");}

    lang = "FILEIS.C";
    printf("\nTestcase %2d: shl_set_language()  ", ++tcno);
    if ((res = shl_set_language(lang)) == SHL_DONE)
    {++passed; printf("Passed");} else {++failed; printf("Failed");}

    mlcs = "/*";
    printf("\nTestcase %2d: is_mlcs()           ", ++\tcno);
    if ((res = is_mlcs(mlcs)) == 2)
    {++passed; printf("Passed");} else {++failed; printf("Failed");}

    mlce = "*/";
    printf("\nTestcase %2d: is_mlce()           ", ++\tcno);
    if ((res = is_mlce(mlce)) == 2)
    {++passed; printf("Passed");} else {++failed; printf("Failed");}

    printf("\nTestcase %2d: is_mlce()           ", ++\tcno);
    if ((res = is_mlce(&mlce[1])) == 0)
    {++passed; printf("Passed");} else {++failed; printf("Failed");}

    slc = "//";
    printf("\nTestcase %2d: is_slc()            ", ++\tcno);
    if ((res = is_slc(slc)) == 2)
    {++passed; printf("Passed");} else {++failed; printf("Failed");}

    str = "\"";
    printf("\nTestcase %2d: is_str()            ", ++\tcno);
    if ((res = is_str(str)))
    {++passed; printf("Passed");} else {++failed; printf("Failed");}

    printf("\nTestcase %2d: is_keyw()           ", ++\tcno);
    if ((res = is_keyw("bool four=true;")) == 4)
    {++passed; printf("Passed");} else {++failed; printf("Failed");}

    printf("\nTestcase %2d: Verify that this looks correct:\n", ++tcno);
    code1 = "\nmain(argc,argv) // switch else case\n{\n  int intchar=0xaf;\n  float pi_float=3.14;\n  printf(\"Hello \"world\"!\");\n  /* Here starts a\n     multi-line comment\n";
    code2 = "     ending here!\n   */\n  if (sizeof(pi_float) > .3)\n    intchar = 67890;\n  return intchar;\n}\n\n";
    buf_b = buf_p = code1;
    buf_e = buf_b + strlen(code1);
    res = shl_highlight(buf_b,buf_e,buf_p,10);
    buf_b = buf_p = code2;
    buf_e = buf_b + strlen(code2);
    res = shl_highlight(buf_b,buf_e,buf_p,10);
}

void suite2() {
    char *lang, *slc, *code1;
    char *buf_b, *buf_e, *buf_p;
    int res = 0;

    lang = "FILEIS.SUB";
    printf("\nTestcase %2d: shl_set_language()  ", ++tcno);
    if ((res = shl_set_language(lang)) == SHL_DONE)
    {++passed; printf("Passed");} else {++failed; printf("Failed");}

    printf("\nTestcase %2d: Verify that this looks correct:\n", ++tcno);
    code1 = "\n; TYPE FILE.TXT\nTYPE FILE.TXT\ndir *.lib\n";
    buf_b = buf_p = code1;
    buf_e = buf_b + strlen(code1);
    res = shl_highlight(buf_b,buf_e,buf_p,10);
}

int main(argc, argv) int argc; char* argv[]; {

    printf("Copyright (C) 2017 by Lars Lindehaven.\n");
    printf("Test of modules for Syntax Highlighter.\n");

    suite1();
    suite2();

    if (!failed)
        printf("\nSuccess! %d of %d tests passed.\n", passed, passed+failed);
    else
        printf("\nFailure! %d of %d tests failed.\n", failed, passed+failed);
}
