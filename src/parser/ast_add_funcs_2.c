#include <stdio.h>

#include "ast.h"
#include "bits/types/timer_t.h"

void prefix_add_next(struct AST *prefix, struct AST *to_add)
{
    if (!prefix->value->prefix->next)
    {
        prefix->value->prefix->next = to_add;
        return;
    }

    struct AST *iterator = prefix->value->redirection->next;

    while (iterator->value->redirection->next)
        iterator = iterator->value->redirection->next;

    iterator->value->redirection->next = to_add;
}

void simple_command_add_prefix(struct AST *simple_command, struct AST *prefix)
{
    if (!simple_command->value->simple_command->prefix)
    {
        simple_command->value->simple_command->prefix = prefix;
    }
}

void word_list_add(struct word_list *word_list, char *word)
{
    struct word_list *new_word_list = word_list_init(word);
    struct word_list *iterator = word_list;

    while (iterator->next)
        iterator = iterator->next;

    iterator->next = new_word_list;
}

void ast_add_redirection(struct AST *ast, struct AST *redirection)
{
    switch (ast->type)
    {
    case COMMAND:
        command_add_redirection(ast, redirection);
        break;
    case PREFIX:
        ast->value->prefix->redirection = redirection;
        break;
    case ELEMENT:
        ast->value->element->redirection = redirection;
        break;
    default:
        fprintf(stderr, "tried to add a redirection to the wrong node");
        break;
    }
}

void case_clause_add_case_item(struct AST *case_clause, struct AST *case_item)
{
    if (!case_clause->value->case_clause->case_item)
    {
        case_clause->value->case_clause->case_item = case_item;
        return;
    }

    struct AST *iterator = case_clause->value->case_clause->case_item;
    while (iterator->value->case_item->next)
        iterator = iterator->value->case_item->next;

    iterator->value->case_item->next = case_item;
    return;
}
