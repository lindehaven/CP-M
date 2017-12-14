/* Copyright (C) 2017 by Lars Lindehaven
 *  Copies n characters from memory area str2 to memory area str1.
 *  Returns pointer to destination, which is str1.
 */
void *memcpy(str1,str2,n)
void *str1; void *str2; unsigned int n;
{
    unsigned int i;
    char *csrc = (char *)str2;
    char *cdest = (char *)str1;
 
    for (i = 0; i < n; i++)
        cdest[i] = csrc[i];

    return str1;
}
