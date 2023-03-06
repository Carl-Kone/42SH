#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../main/42sh.h"
#include "builtins.h"

int builtin_dot(char *command_name, char **argv,
                struct interpreter *interpreter)
{
    assert(interpreter == interpreter);
    int command_name_is_dot = !strcmp(command_name, ".");
    if (command_name_is_dot && !argv[1])
    {
        fprintf(stdin, ".: usage: . filename [arguments]\n");
        return 2;
    }
    else if (command_name_is_dot)
    {
        command_name = argv[1];
    }

    int offset = 0;
    int has_slash = command_name[1] == '/';

    if (command_name[0] == '.')
        offset++;
    if (has_slash)
        offset++;

    char *name_copy = strdup(command_name + offset);

    if (!has_slash)
    {
        char *path = getenv("PATH");
        path = strcat(path, "/");
        char *pathed_name = strcat(path, name_copy);
        free(name_copy);
        name_copy = pathed_name;
    }

    free(argv[0]);
    argv[0] = name_copy;
    return script(compute_argc(argv), argv, 0);
}
