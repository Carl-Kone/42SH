#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"

int builtin_true(char *command_name, char **argv)
{
    assert(command_name == command_name);
    assert(argv == argv);
    return 0;
}

int builtin_false(char *command_name, char **argv)
{
    assert(command_name == command_name);
    assert(argv == argv);
    return 1;
}
