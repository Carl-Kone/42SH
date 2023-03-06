#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#include "builtins.h"

static bool is_integer(char *str)
{
    size_t length = strlen(str);
    for (size_t i = 0; i < length; i++)
    {
        if (!isdigit(str[i]))
            return false;
    }

    return true;
}

int builtin_break(char *command_name, char **argv, int *nb_breaks)
{
    assert(command_name == command_name);
    if (argv[1] != NULL)
    {
        if (!is_integer(argv[1]))
        {
            printf("42sh: break: %s: numeric argument required\n", argv[1]);
            exit(128);
        }
        else
        {
            int n = atoi(argv[1]);
            if (n == 0)
            {
                printf("42sh: break: 0: loop count out of range\n");
                exit(1);
            }
            *nb_breaks = *nb_breaks + n;
        }
    }
    else
        *nb_breaks = *nb_breaks + 1;

    return 0;
}
