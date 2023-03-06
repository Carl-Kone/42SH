#include "interpreter.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../builtins/builtins.h"
#include "../parser/ast_copy.h"
#include "environment.h"
#include "function_list.h"
#include "redirections.h"
#include "variable_list.h"

struct interpreter *interpreter_init(struct environment *env)
{
    struct interpreter *interpreter = calloc(1, sizeof(struct interpreter));
    interpreter->environment = env;
    return interpreter;
}

static char **build_argv(char *command_name, struct AST_element *elements,
                         struct environment *environment)
{
    struct AST_element *current = elements;
    int num_argv = 1;
    char **argv = malloc(num_argv * sizeof(char *));
    argv[0] = command_name;

    while (current && !current->redirection)
    {
        num_argv++;
        argv = realloc(argv, num_argv * sizeof(char *));

        char *expanded = environment_expansion(environment, current->word);
        if (!expanded)
            num_argv--;
        else
            argv[num_argv - 1] = expanded;

        if (!current->next)
            current = NULL;
        else
        {
            assert(current->next->type == ELEMENT);
            current = current->next->value->element;
        }
    }

    num_argv++;
    argv = realloc(argv, num_argv * sizeof(char *));
    argv[num_argv - 1] = NULL;

    return argv;
}

/// Try to execute the command as builtin.
/// If not found, return false, else true.
static bool execute_builtin(char *command_name, char **argv, int *exit_code,
                            struct interpreter *interpreter)
{
    if (!strcmp(command_name, "exit"))
    {
        *exit_code = builtin_exit(command_name, argv, interpreter);
        exit(*exit_code);
    }
    if (strcmp(command_name, "echo") == 0)
        *exit_code = builtin_echo(command_name, argv);
    else if (strcmp(command_name, "true") == 0)
        *exit_code = builtin_true(command_name, argv);
    else if (strcmp(command_name, "false") == 0)
        *exit_code = builtin_false(command_name, argv);
    else if (command_name && command_name[0] == '.')
        *exit_code = builtin_dot(command_name, argv, interpreter);
    else if (strcmp(command_name, "break") == 0)
        *exit_code =
            builtin_break(command_name, argv, &(interpreter->nb_breaks));
    else if (strcmp(command_name, "continue") == 0)
        *exit_code =
            builtin_continue(command_name, argv, &(interpreter->nb_continues));
    else if (!strcmp(command_name, "export"))
        *exit_code = builtin_export(command_name, argv, interpreter);
    else if (!strcmp(command_name, "unset"))
    {
        int tmp = builtin_unset(command_name, argv, interpreter);
        if (tmp == 2 && !interpreter->environment->is_interactive)
        {
            fprintf(stderr, "error in unset while in non-interactive shell\n");
            exit(2);
        }
        *exit_code = tmp;
    }
    else if (!strcmp(command_name, "cd"))
        *exit_code = builtin_cd(command_name, argv, interpreter);
    else
        return false;

    fflush(stdout);
    return true;
}

static int execute_command(char *command_name, char **argv,
                           struct interpreter *interpreter)
{
    int exit_code = 0;
    struct AST *ast = function_list_get(interpreter->environment->function_list,
                                        command_name);
    if (ast)
    {
        exit_code = interprete_ast(ast, interpreter);
        return exit_code;
    }

    if (execute_builtin(command_name, argv, &exit_code, interpreter))
    {
        return exit_code;
    }

    // Do not remove
    fflush(stdin);
    int pid = fork();

    if (pid == 0)
    {
        execvp(command_name, argv);
        fprintf(stderr, "%s: command not found\n", command_name);
        exit(127);
    }
    else
    {
        int status = 0;
        waitpid(pid, &status, 0);
        exit_code = WEXITSTATUS(status);
    }

    return exit_code;
}

static int interprete_else(struct AST_else *ast_else,
                           struct interpreter *interpreter)
{
    return interprete_ast(ast_else->compound_list, interpreter);
}

static int interprete_if(struct AST_if *ast_if, struct interpreter *interpreter)
{
    if (interprete_ast(ast_if->if_compound, interpreter) == 0)
        return interprete_ast(ast_if->then_compound, interpreter);
    else if (ast_if->else_clause)
        return interprete_ast(ast_if->else_clause, interpreter);

    return 0;
}

static int subshell_fork(struct AST *subshell, struct interpreter *interpreter)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        int exit_value = interprete_ast(subshell, interpreter);
        exit(exit_value);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        int exit_code = WEXITSTATUS(status);
        char *exit = calloc(1, 4);
        sprintf(exit, "%d", exit_code);
        variable_list_assign(interpreter->environment->variable_list,
                             strdup("?"), exit);

        return exit_code;
    }
    assert("Never here (subshell_fork)" && false);
    return 0;
}

static int interprete_shell_command(struct AST_shell_command *ast_shell_command,
                                    struct interpreter *interpreter)
{
    if (ast_shell_command->rule_if)
        return interprete_ast(ast_shell_command->rule_if, interpreter);

    if (ast_shell_command->rule_until)
        return interprete_ast(ast_shell_command->rule_until, interpreter);

    if (ast_shell_command->rule_while)
        return interprete_ast(ast_shell_command->rule_while, interpreter);

    if (ast_shell_command->command_block)
        return interprete_ast(ast_shell_command->command_block, interpreter);

    if (ast_shell_command->subshell)
        return subshell_fork(ast_shell_command->subshell, interpreter);

    if (ast_shell_command->rule_case)
        return interprete_ast(ast_shell_command->rule_case, interpreter);

    return interprete_ast(ast_shell_command->rule_for, interpreter);
}

static int interprete_compound_list(struct AST_compound_list *ast_compound_list,
                                    struct interpreter *interpreter)
{
    return interprete_ast(ast_compound_list->and_or, interpreter);
}

static int interprete_input(struct AST_input *ast_input,
                            struct interpreter *interpreter)
{
    return interprete_ast(ast_input->list, interpreter);
}

static int interprete_list(struct AST_list *ast_list,
                           struct interpreter *interpreter)
{
    return interprete_ast(ast_list->and_or, interpreter);
}

static int
interprete_simple_command(struct AST_simple_command *ast_simple_command,
                          struct interpreter *interpreter)
{
    interprete_ast(ast_simple_command->prefix, interpreter);

    // TODO ask theo if this should be moved
    // under.
    char *command_name = environment_expansion(
        interpreter->environment, ast_simple_command->command_name);

    if (!command_name)
        return 0;

    // Redirection.
    interprete_ast(ast_simple_command->element, interpreter);

    // === EXECUTION OF COMMANDS === START

    struct AST_element *element = ast_simple_command->element
        ? ast_simple_command->element->value->element
        : NULL;

    char **argv = build_argv(command_name, element, interpreter->environment);
    int exit_code = execute_command(command_name, argv, interpreter);

    // Free the elements array
    for (size_t i = 0; argv[i]; i++)
        free(argv[i]);

    free(argv);
    // === EXECUTION OF COMMANDS === END

    char *exit = calloc(1, 4);
    sprintf(exit, "%d", exit_code);
    variable_list_assign(interpreter->environment->variable_list, strdup("?"),
                         exit);

    revert_stream(interpreter->environment);
    return exit_code;
}

static int interprete_element(struct AST_element *ast_element,
                              struct interpreter *interpreter)
{
    if (!ast_element)
        return 0;

    interprete_ast(ast_element->redirection, interpreter);
    return interprete_ast(ast_element->next, interpreter);
}

static int interprete_and_or(struct AST_and_or *ast_and_or,
                             struct interpreter *interpreter)
{
    if (!ast_and_or->next)
        return interprete_ast(ast_and_or->pipeline, interpreter);

    interprete_ast(ast_and_or->pipeline, interpreter);
    return interprete_ast(ast_and_or->next, interpreter);
}

static int command_fork(int in, int out, struct AST *command,
                        struct interpreter *interpreter)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        if (in != 0)
        {
            dup2(in, 0);
            close(in);
        }

        if (out != 1)
        {
            dup2(out, 1);
            close(out);
        }

        int exit_value = interprete_ast(command, interpreter);
        exit(exit_value);
    }

    int exitcode = -1;

    close(out);

    waitpid(pid, &exitcode, 0);
    return exitcode;
}

static int execute_pipeline(struct AST_pipeline *ast_pipeline,
                            struct interpreter *interpreter)
{
    int in = 0;
    int fds[2];
    struct AST *iterator = ast_pipeline->command;
    for (; iterator->value->command->next_command;
         iterator = iterator->value->command->next_command)
    {
        pipe(fds);

        int exitcode = command_fork(in, fds[1], iterator, interpreter);

        if (exitcode == -1)
            fprintf(stderr, "error\n");

        in = fds[0];
    }

    if (in != 0)
        dup2(in, 0);

    return ast_pipeline->negation ? !interprete_ast(iterator, interpreter)
                                  : interprete_ast(iterator, interpreter);
}

static int interprete_pipeline(struct AST_pipeline *pipeline,
                               struct interpreter *interpreter)
{
    int left = execute_pipeline(pipeline, interpreter);
    struct AST *right_iterator = pipeline->next_pipeline;
    while (right_iterator)
    {
        // jules if u have any questions pls dont ask us
        switch (right_iterator->value->pipeline->and_or_type)
        {
        case AND:
            left = left
                || execute_pipeline(right_iterator->value->pipeline,
                                    interpreter);
            break;
        case OR:
            left = left
                && execute_pipeline(right_iterator->value->pipeline,
                                    interpreter);
            break;
        case NOTHING:
            return left;
        }
        right_iterator = right_iterator->value->pipeline->next_pipeline;
    }
    return left;
}

static int interprete_command(struct AST_command *ast_command,
                              struct interpreter *interpreter)
{
    interprete_ast(ast_command->redirection, interpreter);
    interprete_ast(ast_command->funcdec, interpreter);
    return interprete_ast(ast_command->simple_or_shell_command, interpreter);
}

static int interprete_redirection(struct AST_redirection *redirection,
                                  struct interpreter *interpreter)
{
    assert(redirection == redirection);
    assert(interpreter == interpreter);
    redirect(redirection, interpreter);
    interprete_ast(redirection->next, interpreter);
    return 0;
}

static int interprete_prefix(struct AST_prefix *prefix,
                             struct interpreter *interpreter)
{
    if (prefix->assignment_word)
        assign_variable(interpreter->environment, prefix->assignment_word);

    if (!prefix->next)
        return interprete_ast(prefix->redirection, interpreter);

    interprete_ast(prefix->redirection, interpreter);
    return interprete_ast(prefix->next, interpreter);
}

static int interprete_while(struct AST_rule_while *rule_while,
                            struct interpreter *interpreter)

{
    interpreter->nb_loops++;
    int res = 0;
    while ((res = interprete_ast(rule_while->condition, interpreter)) == 0
           || res == -1)
    {
        if (interpreter->nb_breaks > 0)
        {
            interpreter->nb_breaks--;
            interpreter->nb_loops--;
            break;
        }

        if (interpreter->nb_loops != 1 && interpreter->nb_continues > 1)
        {
            interpreter->nb_continues--;
            interpreter->nb_loops--;
            break;
        }
        else
            interpreter->nb_continues = 0;

        interprete_ast(rule_while->to_execute, interpreter);
    }

    return 0;
}

static int interprete_until(struct AST_rule_until *rule_until,
                            struct interpreter *interpreter)
{
    interpreter->nb_loops++;
    int res = 1;
    while ((res = interprete_ast(rule_until->condition, interpreter)) != 0
           || res == -1)
    {
        if (interpreter->nb_breaks > 0)
        {
            interpreter->nb_breaks--;
            interpreter->nb_loops--;
            break;
        }

        if (interpreter->nb_loops != 1 && interpreter->nb_continues > 1)
        {
            interpreter->nb_continues--;
            interpreter->nb_loops--;
            break;
        }
        else
            interpreter->nb_continues = 0;

        interprete_ast(rule_until->to_execute, interpreter);
    }

    return 0;
}

static int interprete_for(struct AST_rule_for *rule_for,
                          struct interpreter *interpreter)
{
    if (!rule_for->word_list)
        return 0;
    interpreter->nb_loops++;
    char *name = rule_for->word;
    int exit_code = 0;
    for (struct word_list *iterator = rule_for->word_list; iterator;
         iterator = iterator->next)
    {
        if (interpreter->nb_breaks > 0)
        {
            interpreter->nb_breaks--;
            interpreter->nb_loops--;
            break;
        }
        if (interpreter->nb_loops != 1 && interpreter->nb_continues > 1)
        {
            interpreter->nb_continues--;
            interpreter->nb_loops--;
            break;
        }
        else
            interpreter->nb_continues = 0;

        variable_list_assign(interpreter->environment->variable_list,
                             strdup(name), strdup(iterator->word));
        exit_code = interprete_ast(rule_for->to_execute, interpreter);
    }

    return exit_code;
}

static int interprete_funcdec(struct AST_funcdec *funcdec,
                              struct interpreter *interpreter)
{
    function_list_assign(interpreter->environment->function_list,
                         strdup(funcdec->name),
                         ast_copy(funcdec->shell_command));
    return 0;
}

static int case_cmd(struct AST_case_item *case_item,
                    struct interpreter *interpreter, char *word)
{
    struct word_list *curr = case_item->word_list;

    while (curr)
    {
        if (strcmp(word, curr->word) == 0)
        {
            return interprete_ast(case_item->compound_list, interpreter);
        }

        curr = curr->next;
    }

    if (!case_item->next)
        return 0;

    return case_cmd(case_item->next->value->case_item, interpreter, word);
}

static int interprete_rule_case(struct AST_rule_case *rule_case,
                                struct interpreter *interpreter)
{
    char *word =
        environment_expansion(interpreter->environment, rule_case->word);
    int exit_code = 0;

    if (rule_case->case_clause)
    {
        exit_code = case_cmd(rule_case->case_clause->value->case_clause
                                 ->case_item->value->case_item,
                             interpreter, word);
    }

    free(word);
    return exit_code;
}

static int interprete_ast_2(struct AST *ast, struct interpreter *interpreter)
{
    switch (ast->type)
    {
    case SHELL_COMMAND:
        return interprete_shell_command(ast->value->shell_command, interpreter);
    case IF:
    case ELIF:
        return interprete_if(ast->value->rule_if, interpreter);
    case ELSE:
        return interprete_else(ast->value->else_clause, interpreter);
    case COMPOUND_LIST:
        return interprete_compound_list(ast->value->compound_list, interpreter);
    case PREFIX:
        return interprete_prefix(ast->value->prefix, interpreter);
    case REDIRECTION:
        return interprete_redirection(ast->value->redirection, interpreter);
    case UNTIL:
        return interprete_until(ast->value->rule_until, interpreter);
    case WHILE:
        return interprete_while(ast->value->rule_while, interpreter);
    case FOR:
        return interprete_for(ast->value->rule_for, interpreter);
    case FUNCTION:
        return interprete_funcdec(ast->value->funcdec, interpreter);
    case RULE_CASE:
        return interprete_rule_case(ast->value->rule_case, interpreter);
    default:
        printf("not yet implemented!\n");
        exit(2);
    }
}

int interprete_ast(struct AST *ast, struct interpreter *interpreter)
{
    if (!ast)
        return 0;

    if (interpreter->nb_breaks > 0 && interpreter->nb_loops != 0)
        return -1;
    else
        interpreter->nb_breaks = 0;

    if (interpreter->nb_continues > 0 && interpreter->nb_loops != 0)
    {
        return -1;
    }
    else
        interpreter->nb_continues = 0;

    switch (ast->type)
    {
    case INPUT:
        return interprete_input(ast->value->input, interpreter);
    case LIST:
        return interprete_list(ast->value->list, interpreter);
    case SIMPLE_COMMAND:
        return interprete_simple_command(ast->value->simple_command,
                                         interpreter);
    case ELEMENT:
        return interprete_element(ast->value->element, interpreter);
    case AND_OR:
        return interprete_and_or(ast->value->and_or, interpreter);
    case PIPELINE:
        return interprete_pipeline(ast->value->pipeline, interpreter);
    case COMMAND:
        return interprete_command(ast->value->command, interpreter);
    default:
        return interprete_ast_2(ast, interpreter);
    }

    return 0;
}

void interpreter_free(struct interpreter *interpreter)
{
    free(interpreter);
}
