#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "builtins.h"

struct echo
{
    bool newline;
    bool interprete;
};

static bool valid_opt(char *s, struct echo *echo)
{
    if (!s || s[0] != '-')
        return false;

    for (size_t i = 1; s[i]; i++)
    {
        if (s[i] != 'E' && s[i] != 'n' && s[i] != 'e')
            return false;

        if (s[i] == 'e')
            echo->interprete = true;

        if (s[i] == 'n')
            echo->newline = false;

        if (s[i] == 'E')
            echo->interprete = false;
    }

    return true;
}

static int recognize_or_spell_escape_sequence(char c, bool should_spell,
                                              char next)
{
    if (!c || !next || c != '\\')
        return 0;

    if (should_spell)
        printf("\\");

    switch (next)
    {
    case '\\':
        printf("%c", '\\');
        return 1;
    case 'n':
        printf("%c", should_spell ? 'n' : '\n');
        return 1;
    case 't':
        printf("%c", should_spell ? 't' : '\t');
        return 1;
    default:
        printf("%c", next);
        return 1;
    }
}

static bool print_word(char *str, struct echo *echo)
{
    if (!str)
        return false;

    for (size_t i = 0; str[i]; i++)
    {
        int is_escape = recognize_or_spell_escape_sequence(
            str[i], !(echo->interprete), str[i + 1]);

        if (is_escape == 1)
        {
            i++;
            continue;
        }

        printf("%c", str[i]);
    }
    return true;
}

int builtin_echo(char *command_name, char **argv)
{
    assert(command_name == command_name);
    struct echo echo = { true, false };
    int i = 1;

    while (valid_opt(argv[i], &echo))
        i++;

    if (!argv[i])
        goto exit_and_print;

    for (; argv[i + 1]; i++)
    {
        if (!print_word(argv[i], &echo))
            return 0;
        printf(" ");
    }

    print_word(argv[i], &echo);

exit_and_print:
    if (echo.newline)
        printf("\n");

    return 0;
}
