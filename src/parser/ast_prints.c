#include <stdio.h>

#include "ast.h"

static void rule_case_print(struct AST_rule_case *rule_case)
{
    if (!rule_case)
        return;

    printf("case %s in ", rule_case->word);
    ast_print(rule_case->case_clause);
    printf("esac\n");
}

static void case_clause_print(struct AST_case_clause *case_clause)
{
    if (!case_clause)
        return;
    struct AST *iterator = case_clause->case_item;
    ast_print(iterator);
    iterator = iterator->value->case_item->next;
    while (iterator)
    {
        printf(" ;; ");
        ast_print(iterator);
        iterator = iterator->value->case_item->next;
    }
}

static void case_item_print(struct AST_case_item *case_item)
{
    if (!case_item)
        return;
    struct word_list *word_list = case_item->word_list;
    printf("%s ", word_list->word);
    word_list = word_list->next;
    while (word_list)
    {
        printf("| %s", word_list->word);
        word_list = word_list->next;
    }
    ast_print(case_item->compound_list);
}

static void element_print(struct AST_element *element)
{
    if (!element)
        return;
    if (element->redirection)
        ast_print(element->redirection);
    else
        printf("%s ", element->word);

    ast_print(element->next);
}

static void simple_command_print(struct AST_simple_command *sc)
{
    ast_print(sc->prefix);
    if (sc->command_name)
    {
        printf("%s ", sc->command_name);
        ast_print(sc->element);
    }
}
static void command_print(struct AST_command *command)
{
    ast_print(command->simple_or_shell_command);
    ast_print(command->funcdec);
    ast_print(command->redirection);
    if (command->next_command)
    {
        printf("| ");
        ast_print(command->next_command);
    }
}

static void pipeline_print(struct AST_pipeline *pipeline)
{
    if (pipeline->and_or_type == AND)
        printf("&& ");
    else if (pipeline->and_or_type == OR)
        printf("|| ");

    if (pipeline->negation)
        putchar('!');
    ast_print(pipeline->command);
    ast_print(pipeline->next_pipeline);
    // pipeline is printed in command if there is a next
}

static void and_or_print(struct AST_and_or *and_or)
{
    if (!and_or)
        return;

    ast_print(and_or->pipeline);
    printf("\n");
    ast_print(and_or->next);
}

static void list_print(struct AST_list *list)
{
    if (!list)
        return;

    ast_print(list->and_or);
    printf("\n");
}

static void input_print(struct AST_input *input)
{
    ast_print(input->list);
}

static void compound_list_print(struct AST_compound_list *compound_list)
{
    ast_print(compound_list->and_or);
}

static void rule_if_print(struct AST_if *rule_if)
{
    ast_print(rule_if->if_compound);
    printf("then\n");
    ast_print(rule_if->then_compound);
    ast_print(rule_if->else_clause);
}

static void else_clause_print(struct AST_else *else_clause)
{
    if (!else_clause)
        return;
    printf("else ");
    ast_print(else_clause->compound_list);
}

static void while_print(struct AST_rule_while *rule_while)
{
    if (!rule_while)
        return;
    printf("while ");
    ast_print(rule_while->condition);
    printf(" do ");
    ast_print(rule_while->to_execute);
    printf("done ");
}

static void until_print(struct AST_rule_until *rule_until)
{
    if (!rule_until)
        return;
    printf("until ");
    ast_print(rule_until->condition);
    printf(" do ");
    ast_print(rule_until->to_execute);
    printf("done ");
}

static void for_print(struct AST_rule_for *rule_for)
{
    if (!rule_for)
        return;
    printf("for %s ", rule_for->word);
    if (rule_for->word_list)
        printf("there is a word list ");
    printf("do ");
    ast_print(rule_for->to_execute);
    printf("done");
}

static void shell_command_print(struct AST_shell_command *shell_command)
{
    if (shell_command->command_block)
    {
        printf("{\n");
        ast_print(shell_command->command_block);
        putchar('}');
    }
    else if (shell_command->subshell)
    {
        printf("(\n");
        ast_print(shell_command->subshell);
        putchar(')');
    }
    ast_print(shell_command->rule_if);
    ast_print(shell_command->rule_while);
    ast_print(shell_command->rule_until);
    ast_print(shell_command->rule_for);
    ast_print(shell_command->rule_case);
}

static void funcdec_print(struct AST_funcdec *funcdec)
{
    printf("%s()", funcdec->name);
    ast_print(funcdec->shell_command);
}

static void redirection_print_sign(enum REDIRECTION_TYPE type)
{
    switch (type)
    {
    case REDIRECTING_INPUT:
        printf("<");
        break;
    case REDIRECTING_OUTPUT:
        printf(">");
        break;
    case CLOBBER:
        printf(">|");
        break;
    case APPEND_OUTPUT:
        printf(">>");
        break;
    case DUPLICATE_INPUT:
        printf("<&");
        break;
    case DUPLICATE_OUTPUT:
        printf(">&");
        break;
    case OPEN_FD:
        printf("<>");
        break;
    default:
        printf("error???????");
        break;
    }
}

static void redirection_print(struct AST_redirection *redirection)
{
    if (redirection->io_number != -1)
        printf("%d", redirection->io_number);

    redirection_print_sign(redirection->type);
    printf(" %s ", redirection->word);
    ast_print(redirection->next);
}

static void prefix_print(struct AST_prefix *prefix)
{
    if (prefix->assignment_word)
        printf("%s ", prefix->assignment_word);
    else
        ast_print(prefix->redirection);
    ast_print(prefix->next);
}

static void ast_print_2(struct AST *ast)
{
    if (!ast)
        return;
    switch (ast->type)
    {
    case SHELL_COMMAND:
        shell_command_print(ast->value->shell_command);
        break;
    case COMPOUND_LIST:
        compound_list_print(ast->value->compound_list);
        break;
    case REDIRECTION:
        redirection_print(ast->value->redirection);
        break;
    case PREFIX:
        prefix_print(ast->value->prefix);
        break;
    case WHILE:
        while_print(ast->value->rule_while);
        break;
    case UNTIL:
        until_print(ast->value->rule_until);
        break;
    case FOR:
        for_print(ast->value->rule_for);
        break;
    case FUNCTION:
        funcdec_print(ast->value->funcdec);
        break;
    case RULE_CASE:
        rule_case_print(ast->value->rule_case);
        break;
    case CASE_CLAUSE:
        case_clause_print(ast->value->case_clause);
        break;
    case CASE_ITEM:
        case_item_print(ast->value->case_item);
        break;
    default:
        printf("not yet implemented!\n");
    }
}

void ast_print(struct AST *ast)
{
    if (!ast)
        return;

    switch (ast->type)
    {
    case INPUT:
        input_print(ast->value->input);
        break;
    case ELEMENT:
        element_print(ast->value->element);
        break;
    case SIMPLE_COMMAND:
        simple_command_print(ast->value->simple_command);
        break;
    case PIPELINE:
        pipeline_print(ast->value->pipeline);
        break;
    case COMMAND:
        command_print(ast->value->command);
        break;
    case AND_OR:
        and_or_print(ast->value->and_or);
        break;
    case LIST:
        list_print(ast->value->list);
        break;
    case IF:
        printf("if\n");
        rule_if_print(ast->value->rule_if);
        break;
    case ELSE:
        else_clause_print(ast->value->else_clause);
        break;
    default:
        ast_print_2(ast);
    }
}
