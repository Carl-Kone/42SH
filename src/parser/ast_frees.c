#include <assert.h>
#include <stdlib.h>

#include "ast.h"

static void rule_case_free(struct AST_rule_case *rule_case)
{
    free(rule_case->word);
    ast_free(rule_case->case_clause);
    free(rule_case);
}

static void case_clause_free(struct AST_case_clause *case_clause)
{
    ast_free(case_clause->case_item);
    free(case_clause);
}

static void case_item_free(struct AST_case_item *item_case)
{
    word_list_free(item_case->word_list);
    ast_free(item_case->next);
    ast_free(item_case->compound_list);
    free(item_case);
}

static void element_free(struct AST_element *element)
{
    if (!element)
        return;

    free(element->word);
    ast_free(element->redirection);
    ast_free(element->next);
    free(element);
}

static void simple_command_free(struct AST_simple_command *sc)
{
    if (!sc)
        return;

    free(sc->command_name);
    ast_free(sc->element);
    ast_free(sc->prefix);
    free(sc);
}

static void command_free(struct AST_command *command)
{
    ast_free(command->simple_or_shell_command);
    ast_free(command->redirection);
    ast_free(command->funcdec);
    ast_free(command->next_command);
    free(command);
}

static void pipeline_free(struct AST_pipeline *pipeline)
{
    ast_free(pipeline->command);
    ast_free(pipeline->next_pipeline);
    free(pipeline);
}

static void and_or_free(struct AST_and_or *and_or)
{
    if (!and_or)
        return;

    ast_free(and_or->pipeline);
    ast_free(and_or->next);
    free(and_or);
}

static void list_free(struct AST_list *list)
{
    ast_free(list->and_or);
    free(list);
}

static void input_free(struct AST_input *input)
{
    ast_free(input->list);
    free(input);
}

static void if_free(struct AST_if *rule_if)
{
    ast_free(rule_if->if_compound);
    ast_free(rule_if->then_compound);
    ast_free(rule_if->else_clause);
    free(rule_if);
}

static void else_free(struct AST_else *else_clause)
{
    ast_free(else_clause->compound_list);
    free(else_clause);
}

static void shell_command_free(struct AST_shell_command *shell_command)
{
    ast_free(shell_command->rule_if);
    ast_free(shell_command->rule_while);
    ast_free(shell_command->rule_until);
    ast_free(shell_command->rule_for);
    ast_free(shell_command->command_block);
    ast_free(shell_command->subshell);
    ast_free(shell_command->rule_case);
    free(shell_command);
}

static void compound_list_free(struct AST_compound_list *compound_list)
{
    ast_free(compound_list->and_or);
    free(compound_list);
}

static void redirection_free(struct AST_redirection *redirection)
{
    free(redirection->word);
    ast_free(redirection->next);
    free(redirection);
}

static void prefix_free(struct AST_prefix *prefix)
{
    ast_free(prefix->redirection);
    ast_free(prefix->next);
    free(prefix->assignment_word);
    free(prefix);
}

static void while_free(struct AST_rule_while *rule_while)
{
    ast_free(rule_while->condition);
    ast_free(rule_while->to_execute);
    free(rule_while);
}

static void until_free(struct AST_rule_until *rule_until)
{
    ast_free(rule_until->condition);
    ast_free(rule_until->to_execute);
    free(rule_until);
}

static void for_free(struct AST_rule_for *rule_for)
{
    free(rule_for->word);
    word_list_free(rule_for->word_list);
    ast_free(rule_for->to_execute);
    free(rule_for);
}

void word_list_free(struct word_list *word_list)
{
    if (!word_list)
        return;
    struct word_list *iterator = word_list->next;
    while (word_list)
    {
        iterator = word_list->next;
        free(word_list->word);
        free(word_list);
        word_list = iterator;
    }
}

void funcdec_free(struct AST_funcdec *funcdec)
{
    free(funcdec->name);
    ast_free(funcdec->shell_command);
    free(funcdec);
}

void ast_free_2(struct AST *ast)
{
    if (!ast)
        return;
    switch (ast->type)
    {
    case SHELL_COMMAND:
        shell_command_free(ast->value->shell_command);
        break;
    case COMPOUND_LIST:
        compound_list_free(ast->value->compound_list);
        break;
    case PREFIX:
        prefix_free(ast->value->prefix);
        break;
    case REDIRECTION:
        redirection_free(ast->value->redirection);
        break;
    case WHILE:
        while_free(ast->value->rule_while);
        break;
    case UNTIL:
        until_free(ast->value->rule_until);
        break;
    case FOR:
        for_free(ast->value->rule_for);
        break;
    case FUNCTION:
        funcdec_free(ast->value->funcdec);
        break;
    case RULE_CASE:
        rule_case_free(ast->value->rule_case);
        break;
    case CASE_CLAUSE:
        case_clause_free(ast->value->case_clause);
        break;
    case CASE_ITEM:
        case_item_free(ast->value->case_item);
        break;
    default:
        printf("not yet implemented!\n");
    }
}

void ast_free(struct AST *ast)
{
    if (!ast)
        return;

    switch (ast->type)
    {
    case INPUT:
        input_free(ast->value->input);
        break;
    case ELEMENT:
        element_free(ast->value->element);
        break;
    case SIMPLE_COMMAND:
        simple_command_free(ast->value->simple_command);
        break;
    case PIPELINE:
        pipeline_free(ast->value->pipeline);
        break;
    case COMMAND:
        command_free(ast->value->command);
        break;
    case AND_OR:
        and_or_free(ast->value->and_or);
        break;
    case LIST:
        list_free(ast->value->list);
        break;
    case IF:
        if_free(ast->value->rule_if);
        break;
    case ELSE:
        else_free(ast->value->else_clause);
        break;
    default:
        ast_free_2(ast);
    }
    free(ast->value);
    free(ast);
}
