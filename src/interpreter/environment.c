#include "environment.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../lexer/lexer.h"
#include "function_list.h"
#include "variable_list.h"

struct environment *environment_init(void)
{
    struct environment *environment = malloc(sizeof(struct environment));
    environment->out = dup(fileno(stdout));
    environment->in = dup(fileno(stdin));
    environment->err = dup(fileno(stderr));
    environment->variable_list = variable_list_init();
    environment->function_list = function_list_init();
    environment->is_interactive = true;
    return environment;
}

void environment_free(struct environment *environment)
{
    // TODO close fd;
    variable_list_free(environment->variable_list);
    function_list_free(environment->function_list);
    free(environment);
}

void assign_variable(struct environment *environment, char *input)
{
    // Find the first "=" character in the input string
    char *equals_sign = strchr(input, '=');
    if (equals_sign == NULL)
    {
        assert(false);
    }

    // Split the input string on the "=" character
    char *name = strndup(input, equals_sign - input);
    char *value = strdup(equals_sign + 1);

    // Assign the variable in the environment
    variable_list_assign(environment->variable_list, name, value);
}

char *get_variable_name(char *input)
{
    if (*input == '{')
    {
        char *var_name_end = strchr(input + 1, '}');
        return strndup(input + 1, var_name_end - (input + 1));
    }
    else
    {
        char *var_name_end = strpbrk(input, " $\"\'");
        return strndup(input, var_name_end - input);
    }
}

char *environment_expansion(struct environment *environment, char *input)
{
    if (!input)
        return NULL;

    struct lexer *lexer = lexer_init(input);
    lexer->is_in_expansion_mode = true;
    lexer->environment = environment;

    struct token *token = next_token(lexer, FILTER_WORD);
    if (token_is_eof(token))
    {
        token_free(token);
        lexer_free(lexer);
        return NULL;
    }

    assert(token_is_word(token));
    char *output = strdup(token_get_word_value(token));
    token_free(token);

    token = next_token(lexer, FILTER_WORD);
    assert(token_is_eof(token));
    token_free(token);

    lexer_free(lexer);
    return output;
}
