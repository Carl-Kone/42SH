#include "utils.h"

#include <stddef.h>
#include <stdlib.h>

char *fileToString(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    size_t length = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    char *string = malloc(sizeof(char) * length + 1);
    string[length] = '\0';

    for (size_t i = 0; i < length; i++)
    {
        string[i] = fgetc(fptr);
    }
    return string;
}
