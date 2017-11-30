/*
 *  Syntax Higlighter -- tests highlighted source code
 *
 *  Copyright (C) 2017 Lars Lindehaven <lars dot lindehaven at gmail dot com>
 *  All rights reserved.
 *
 */

#include "shl.h"
#include "stdio.h"

int main(argc, argv) int argc; char* argv[];
{
    int tcno = 0;
    int passed = 0;
    int failed = 0;

    char *code1 = "\nmain(argc,argv) /* main() is not a keyword in this comment */\n{\n  int rc = 12345;\n  printf(\"Hello \"world\"!\");\n  /* Here starts a\n     multi-line comment\n";
    char *code2 = "     ending here!\n   */\n  rc = 67890;\n  return rc;\n}\n\n";
    char *mlcb = "/*";
    char *mlce = "*/";
    char *slc = "//";
    char *str = "\"";
    char *buf_b, *buf_e, *buf_p;
    int res = 0;

    printf("Copyright (C) 2017 by Lars Lindehaven.");
    printf("\n Test of modules for Syntax Highlighter.");

    printf("\n  Testcase %2d: shl_set_language()  ", ++tcno);
    if ((res = shl_set_language("FILEIS.???")) == -1)
    {++passed; printf("Passed");} else {++failed; printf("Failed");}

    printf("\n  Testcase %2d: shl_highlight()     ", ++tcno);
    if ((res = shl_highlight(buf_b,buf_e,buf_p,10)) == -1)
    {++passed; printf("Passed");} else {++failed; printf("Failed");}

    printf("\n  Testcase %2d: shl_set_language()  ", ++tcno);
    if ((res = shl_set_language("FILEIS.C")) == 0)
    {++passed; printf("Passed");} else {++failed; printf("Failed");}

    printf("\n  Testcase %2d: is_mlcb()           ", ++\tcno);
    if ((res = is_mlcb(mlcb)))
    {++passed; printf("Passed");} else {++failed; printf("Failed");}

    printf("\n  Testcase %2d: !is_mlce()          ", ++\tcno);
    if ((res = !is_mlce(mlce)))
    {++passed; printf("Passed");} else {++failed; printf("Failed");}

    printf("\n  Testcase %2d: is_mlce()           ", ++\tcno);
    if ((res = is_mlce(&mlce[1])))
    {++passed; printf("Passed");} else {++failed; printf("Failed");}

    printf("\n  Testcase %2d: is_slc()            ", ++\tcno);
    if ((res = is_slc(slc)))
    {++passed; printf("Passed");} else {++failed; printf("Failed");}

    printf("\n  Testcase %2d: is_str()            ", ++\tcno);
    if ((res = is_str(str)))
    {++passed; printf("Passed");} else {++failed; printf("Failed");}

    printf("\n  Testcase %2d: Verify that this looks correct:\n", ++tcno);
    buf_b = buf_p = code1;
    buf_e = buf_b + strlen(code1);
    res = shl_highlight(buf_b,buf_e,buf_p,10);
    buf_b = buf_p = code2;
    buf_e = buf_b + strlen(code2);
    res = shl_highlight(buf_b,buf_e,buf_p,10);

    if (!failed)
        printf("\n Success! %d of %d tests passed.\n", passed, passed+failed);
    else
        printf("\n Failure! %d of %d tests failed.\n", failed, passed+failed);
}
