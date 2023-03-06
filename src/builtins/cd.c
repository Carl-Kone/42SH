#include <assert.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "builtins.h"

static int set_pwds(struct interpreter *interpreter, char *arg)
{
    char *curr_pwd =
        variable_list_get(interpreter->environment->variable_list, "PWD");

    variable_list_assign(interpreter->environment->variable_list,
                         strdup("OLDPWD"), strdup(curr_pwd));
    if (chdir(arg))
    {
        fprintf(stderr, "42sh: cd: no such file or directory: %s\n", arg);
        free(curr_pwd);
        return 1;
    }
    variable_list_assign(interpreter->environment->variable_list, strdup("PWD"),
                         getcwd(NULL, 0));
    free(curr_pwd);

    return 0;
}

bool builtin_cd(char *command_name, char **argv,
                struct interpreter *interpreter)
{
    if (compute_argc(argv) > 2)
    {
        fprintf(stderr, "42sh: cd: too many arguments\n");
        return 1;
    }

    assert(interpreter == interpreter);
    assert(command_name == command_name);
    if (!argv[1])
    {
        char *home = getenv("HOME");
        if (!home)
        {
            fprintf(stderr, "42sh: cd: HOME environment variable not set!\n");
            return 1;
        }

        return set_pwds(interpreter, home);
    }
    if (!strcmp(argv[1], "~"))
    {
        struct passwd *pwd = getpwuid(getuid());
        return set_pwds(interpreter, pwd->pw_dir);
    }
    else if (!strcmp(argv[1], "."))
    {
        char *pwd =
            variable_list_get(interpreter->environment->variable_list, "PWD");

        int exit_code = set_pwds(interpreter, pwd);
        free(pwd);
        return exit_code;
    }
    else if (!strcmp(argv[1], "-"))
    {
        char *old_pwd = variable_list_get(
            interpreter->environment->variable_list, "OLDPWD");

        printf("%s\n", old_pwd);

        int exit_code = set_pwds(interpreter, old_pwd);
        free(old_pwd);
        return exit_code;
    }
    else
    {
        return set_pwds(interpreter, argv[1]);
    }

    return 0;
}
