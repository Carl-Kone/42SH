#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../interpreter/environment.h"
#include "../interpreter/interpreter.h"
#include "../interpreter/variable_list.h"
#include "../lexer/lexer.h"
#include "../parser/ast_copy.h"
#include "../parser/parser.h"
#include "utils.h"

int sh42(char *str, bool is_in_prompt_mode, struct environment *environment)
{
    struct interpreter *interpreter = interpreter_init(environment);

    // SETUP ENVIRONMENT
    variable_list_assign(environment->variable_list, strdup("?"), strdup("0"));

    struct lexer *lexer = lexer_init(str);
    struct parser *parser = parser_init(lexer, is_in_prompt_mode);
    lexer->is_prompt = is_in_prompt_mode;

    int exit_code = 0;

    while (!parser_all_input_parsed(parser))
    {
        struct AST *ast = parser_parse(parser);

        if (!ast)
        {
            fprintf(stderr, "syntax error!\n");
            exit(2);
        }
        // printf("Ast has been parsed in main loop (PID=%d)\n", getpid());
        if (ast->value->input->list)
            exit_code = interprete_ast(ast, interpreter);

        ast_free(ast);
    }

    parser_free(parser);
    environment_free(environment);
    interpreter_free(interpreter);
    return exit_code == -1 ? 0 : exit_code;
}

static int fetchrows(char **buffer, size_t *size, FILE *file)
{
    *buffer = calloc(1, 1);
    int res = 0;
    *size = 1;
    while ((res = fgetc(file)) != -1 && res != '\n')
    {
        *buffer = realloc(*buffer, *size + 1);
        (*buffer)[*size - 1] = res;
        (*size)++;
    }

    if (res == '\n')
    {
        *buffer = realloc(*buffer, *size + 1);
        (*buffer)[*size - 1] = res;
        (*size)++;
    }
    (*buffer)[(*size) - 1] = 0;

    return res == -1 ? -1 : res;
}

int standard_input(void)
{
    char *buffer = NULL;
    size_t size = 0;
    int res = fetchrows(&buffer, &size, stdin);

    // int res = getline(&buffer, &size, stdin);
    if (res == -1)
    {
        // TODO
        // Be sure it's the right thing to do; we had a segfault before the if
        free(buffer);
        return 0;
    }

    struct environment *environment = environment_init();
    variable_list_assign(environment->variable_list, strdup("#"), strdup("0"));
    variable_list_assign(environment->variable_list, strdup("0"),
                         strdup("42sh"));

    // Call the Lexer with buffer
    // TODO exit code?
    int exit_code = sh42(buffer, true, environment);
    free(buffer);

    return exit_code;
}

void argv_init(int argc, char *argv[], struct environment *environment)
{
    char *arg_str = calloc(1, 100);
    size_t total_len = 0;

    for (int i = 1; i < argc; i++)
    {
        total_len += 2;
        total_len += strlen(argv[i]);
    }

    char *concat_arg = calloc(1, total_len + 3);
    concat_arg[0] = '\'';

    for (int i = 0; i < argc - 1; i++)
    {
        memset(arg_str, 0, 100);
        sprintf(arg_str, "%d", i);
        variable_list_assign(environment->variable_list, strdup(arg_str),
                             strdup(argv[i + 1]));
        if (i > 0)
        {
            strcat(concat_arg, argv[i + 1]);
            strcat(concat_arg, " ");
        }
    }
    int q = strlen(concat_arg) - 1;
    if (q < 1)
        q = 1;
    concat_arg[q] = '\'';
    variable_list_assign(environment->variable_list, strdup("*"), concat_arg);
    free(arg_str);
}

int script(int argc, char *argv[], int path_index)
{
    FILE *fptr = fopen(argv[path_index], "r");
    if (!fptr)
    {
        fprintf(stderr, "File \"%s\" does not exist\n", argv[path_index]);
        exit(2);
    }
    char *script = fileToString(fptr);

    char *num_argv = calloc(1, 100);
    sprintf(num_argv, "%d", argc - path_index - 1);
    struct environment *environment = environment_init();
    variable_list_assign(environment->variable_list, strdup("#"), num_argv);
    argv_init(argc, argv, environment);

    // Call the Lexer with the string script
    int exit_code = sh42(script, false, environment);

    free(script);
    fclose(fptr);
    return exit_code;
}

int string(char *str)
{
    struct environment *environment = environment_init();
    variable_list_assign(environment->variable_list, strdup("#"), strdup("0"));
    variable_list_assign(environment->variable_list, strdup("0"),
                         strdup("42sh"));
    environment->is_interactive = false;

    return sh42(str, false, environment);
}

int main(int argc, char **argv)
{
    // Read from the standard input
    if (argc == 1)
    {
        return standard_input();
    }
    if (!strcmp(argv[1], "-c") && argc == 3)
    {
        return string(argv[2]);
    }
    // Read command from a shell script
    else
    {
        return script(argc, argv, 1);
    }

    return 0;
}
