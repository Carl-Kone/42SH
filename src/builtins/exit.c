#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"

static bool is_integer(char *str)
{
    size_t length = strlen(str);
    for (size_t i = 0; i < length; i++)
    {
        if (!isdigit(str[i]) && str[i] != '-')
            return false;
    }
    return true;
}

int compute_argc(char **argv)
{
    int i = 0;
    while (argv[i])
    {
        i++;
    }

    return i;
}

int builtin_exit(char *command_name, char **argv,
                 struct interpreter *interpreter)
{
    assert(command_name == command_name);
    if (argv[1] && compute_argc(argv) != 2)
    {
        fprintf(stderr, "42sh: exit: too many arguments\n");
        return 1;
    }

    // exit n
    if (argv[1] != NULL)
    {
        if (!is_integer(argv[1]))
        {
            fprintf(stderr, "42sh: %s: numeric argument required\n", argv[1]);
            return 2;
        }
        else
        {
            int exit_code = atoi(argv[1]);
            return exit_code % 256;
        }
    }
    else
    {
        char *str_return_code =
            variable_list_get(interpreter->environment->variable_list, "?");
        int exit_code = atoi(str_return_code);
        free(str_return_code);
        return exit_code;
    }
}
