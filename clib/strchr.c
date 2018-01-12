/* Copyright (C) 2017-2018 by Lars Lindehaven
 *  Searches for the first occurrence of the character c (an unsigned char) in
 *  the string pointed to by the argument str.
 *  Returns a pointer to the first occurence of the character n in the string
 *  str, or NULL if the character is not found.
 */
#ifndef NULL
#define NULL 0
#endif
char *strchr(str,c)
char *str; int c;
{
    while (*str)
    {
        if(*str == c)
            return str;
        str++;
    }
    return NULL;
}
