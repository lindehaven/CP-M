/* Copyright (C) 2017 by Lars Lindehaven
 *  Test of modules for Aztec C.
 */
#include "stdio.h"

int main(argc, argv) int argc; char* argv[];
{
    int tcno = 0;
    int passed = 0;
    int failed = 0;
    char *str1 = "failed of course";
    char *str2 = "passed";
    char *haystack = "This is a huge haystack where needle is found!";
    char pin = 'n';
    char screw = 'Y';
    char *needle = "needle";
    char *nail = "nail";
    char *res = NULL;

    printf("Copyright (C) 2017 by Lars Lindehaven.");
    printf("\n Test of modules for Aztec C.");

    printf("\n  Testcase %2d: ", ++tcno);
    if ((res = memcpy(str1, str2, 4)) != NULL) {++passed; printf("Passed");} else {++failed; printf("Failed");}

    printf("\n  Testcase %2d: ", ++tcno);
    if ((res = strchr(haystack, pin)) != NULL) {++passed; printf("Passed");} else {++failed; printf("Failed");}

    printf("\n  Testcase %2d: ", ++tcno);
    if ((res = strchr(haystack, screw)) == NULL) {++passed; printf("Passed");} else {++failed; printf("Failed");}

    printf("\n  Testcase %2d: ", ++tcno);
    if ((res = strchr(",.()+-/*=~%<>[]:;", '+')) != NULL) {++passed; printf("Passed");} else {++failed; printf("Failed");}
   
    printf("\n  Testcase %2d: ", ++tcno);
    if ((res = strchr(",.()+-/*=~%<>[]:;", '?')) == NULL) {++passed; printf("Passed");} else {++failed; printf("Failed");}

    printf("\n  Testcase %2d: ", ++tcno);
    if ((res = strstr(haystack, needle)) != NULL) {++passed; printf("Passed");} else {++failed; printf("Failed");}

    printf("\n  Testcase %2d: ", ++tcno);
    if ((res = strstr(haystack, nail)) == NULL) {++passed; printf("Passed");} else {++failed; printf("Failed");}

    printf("\n  Testcase %2d: ", ++tcno);
    if ((res = strstr("Lars Lindehaven", "s L")) != NULL) {++passed; printf("Passed");} else {++failed; printf("Failed");}

    if (!failed)
        printf("\n Success! All %d tests passed.\n", passed+failed);
    else
        printf("\n Failure! %d of %d tests failed.\n", failed, passed+failed);
}
