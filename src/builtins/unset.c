#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "builtins.h"

// unset variables
// 0 no arg
// 1 error 1
// 2 error 2
// 3 unset function
// 4 unset variables
static char parse_arg(char *arg)
{
    if (arg[0] != '-' || !strcmp(arg, "-"))
        return 0;

    char res = 4;

    if (arg[1] == 'v')
        res = 4;
    else if (arg[1] == 'f')
        res = 3;
    else
        return 2;

    for (int i = 2; arg[i]; i++)
    {
        if (arg[i] == 'f' && res == 4)
            return 1;
        if (arg[i] == 'v' && res == 3)
            return 1;
        if (arg[i] != 'v' && arg[i] != 'f')
            return 2;
    }
    return res;
}

static int unset_variables(char **argv, struct interpreter *interpreter)
{
    int res = 0;
    for (int i = 1; argv[i]; i++)
    {
        struct variable_list *vl =
            interpreter->environment->variable_list->next;
        while (vl)
        {
            if (!strcmp(argv[i], vl->name))
            {
                free(vl->value);
                vl->value = NULL;
                res = 1;
            }
            vl = vl->next;
        }
    }
    return res;
}

static int unset_functions(char **argv, struct interpreter *interpreter)
{
    for (int i = 1; argv[i]; i++)
    {
        struct function_list *fl =
            interpreter->environment->function_list->next;
        while (fl)
        {
            if (!strcmp(argv[i], fl->name))
            {
                ast_free(fl->value);
                fl->value = NULL;
            }
            fl = fl->next;
        }
    }
    return 0;
}

int builtin_unset(char *command_name, char **argv,
                  struct interpreter *interpreter)
{
    assert(command_name == command_name);
    if (!argv[1])
    {
        fprintf(stderr, "unset: not enough arguments\n");
        return 1;
    }

    // check arguments
    char arg = parse_arg(argv[1]);

    switch (arg)
    {
    case 0:
        if (!unset_variables(argv, interpreter))
            unset_functions(argv, interpreter);
        return 0;
    case 1:
        fprintf(
            stderr,
            "unset: cannot simultaneously unset a function and a variable\n");
        return 1;
    case 2:
        fprintf(stderr, "unset: usage: unset [-f] [-v] [name ...]\n");
        return 2;
    case 3:
        unset_functions(argv, interpreter);
        return 0;
    case 4:
        unset_variables(argv, interpreter);
        return 0;
    default:
        fprintf(stderr, "in unset, shouldn't happen\n");
        return 2;
    }
}
