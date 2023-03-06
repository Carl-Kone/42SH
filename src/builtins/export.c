#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"

static int drive_export(char *arg, struct variable_list *va)
{
    if (!arg || !strcmp(arg, "="))
    {
        fprintf(stderr, "42sh: export: \'%s\': not a valid identifier\n", arg);
        return 1;
    }

    char *var_value = arg;
    char *var_name = strtok_r(arg, "=", &var_value);
    variable_list_assign(va, strdup(var_name), strdup(var_value));
    setenv(var_name, var_value, 1);

    return 0;
}

int builtin_export(char *command_name, char **argv,
                   struct interpreter *interpreter)
{
    assert(command_name == command_name);
    int exit_code = 0;
    for (int i = 1; argv[i]; i++)
    {
        if (drive_export(argv[i], interpreter->environment->variable_list))
            exit_code = 1;
    }

    return exit_code;
}
