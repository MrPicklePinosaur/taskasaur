
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "headers/utils.h"

int
min(int a, int b)
{
    return (a < b) ? a : b;
}

char*
wrap_text(char* str, int max_width, int* lines)
{
    char* wrapped_str;
    char* str_read;
    int totlen;
    int line_count;

    wrapped_str = malloc(sizeof(char));
    wrapped_str[0] = 0;
    str_read = str;
    totlen = 0;
    line_count = 0;

    for (int i = 0; i < floor(strlen(str)/max_width)+1; i++) {

        int curlen;
        
        curlen = min(strlen(str_read), max_width);
        totlen += (curlen+1); // account for new line

        wrapped_str = realloc(wrapped_str, sizeof(char)*totlen+1); // account for null
        strncat(wrapped_str, str_read, curlen);
        strcat(wrapped_str, "\n");

        str_read += max_width;
        line_count += 1;
    }

    *lines = line_count;

    return wrapped_str;

}


