/* Copyright (C) 2017-2018 by Lars Lindehaven
 *  Finds the first occurrence of the substring needle in the string haystack.
 *  The terminating '\0' characters are not compared.
 *  Returns a pointer to the first occurrence in haystack of any of the entire
 *  sequence of characters specified in needle, or a null pointer if the
 *  sequence is not present in haystack.
 */
#ifndef NULL
#define NULL 0
#endif
int compare(cx, cy)
char *cx; char *cy;
{
    while (*cx && *cy)
    {
        if (*cx != *cy)
            return 0;
        cx++;
        cy++;
    }
    return (*cy == '\0');
}

char *strstr(haystack,needle)
char *haystack; char *needle;
{
    while (*haystack != '\0')
    {
        if ((*haystack == *needle) && compare(haystack, needle))
            return haystack;
        haystack++;
    }
    return NULL;
}
