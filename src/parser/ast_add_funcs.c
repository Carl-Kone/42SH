#include <stdio.h>

#include "ast.h"

// root should be an and_or tree and to_add should be a pipeline
void pipeline_add_next(struct AST *root, struct AST *to_add)
{
    if (!root->value->and_or->pipeline)
    {
        root->value->and_or->pipeline = to_add;
        return;
    }

    struct AST *iterator = root->value->and_or->pipeline;
    while (iterator->value->pipeline->next_pipeline)
        iterator = iterator->value->pipeline->next_pipeline;

    iterator->value->pipeline->next_pipeline = to_add;
}

// el is an ast_pipeline and adds the command to the list of commands
void command_add_next(struct AST *el, struct AST *to_add)
{
    if (!el->value->pipeline->command)
    {
        el->value->pipeline->command = to_add;
        return;
    }
    struct AST *iterator = el->value->pipeline->command;
    while (iterator->value->command->next_command)
        iterator = iterator->value->command->next_command;

    iterator->value->command->next_command = to_add;
}

void element_add_next(struct AST *el, struct AST *new_el)
{
    if (!el->value->element->next)
    {
        el->value->element->next = new_el;
        return;
    }

    struct AST *iterator = el->value->element->next;
    while (iterator->value->element->next)
        iterator = iterator->value->element->next;

    iterator->value->element->next = new_el;
}

void simple_command_add_element(struct AST *sc, struct AST *el)
{
    if (!sc->value->simple_command->element)
    {
        sc->value->simple_command->element = el;
        return;
    }

    element_add_next(sc->value->simple_command->element, el);
}

void and_or_add_next(struct AST *and_or, struct AST *to_add)
{
    if (!and_or->value->and_or->next)
    {
        and_or->value->and_or->next = to_add;
        return;
    }

    struct AST *iterator = and_or->value->and_or->next;

    while (iterator->value->and_or->next)
        iterator = iterator->value->and_or->next;

    iterator->value->and_or->next = to_add;
}

void list_add_and_or_next(struct AST *list, struct AST *and_or)
{
    if (!list->value->list->and_or)
    {
        list->value->list->and_or = and_or;
        return;
    }

    and_or_add_next(list->value->list->and_or, and_or);
}

void compound_list_add_and_or_next(struct AST *compound_list,
                                   struct AST *and_or)
{
    if (!compound_list->value->compound_list->and_or)
    {
        compound_list->value->compound_list->and_or = and_or;
        return;
    }
    and_or_add_next(compound_list->value->compound_list->and_or, and_or);
}

void ast_add_and_or_next(struct AST *ast, struct AST *and_or)
{
    switch (ast->type)
    {
    case COMPOUND_LIST:
        compound_list_add_and_or_next(ast, and_or);
        break;
    case LIST:
        list_add_and_or_next(ast, and_or);
        break;
    default:
        printf("not implemented yet!\n");
        break;
    }
}

void add_redirection_next(struct AST *redirection, struct AST *to_add)
{
    if (!redirection->value->redirection->next)
    {
        redirection->value->redirection->next = to_add;
        return;
    }

    struct AST *iterator = redirection->value->redirection->next;

    while (iterator->value->redirection->next)
        iterator = iterator->value->redirection->next;

    iterator->value->redirection->next = to_add;
}

void command_add_redirection(struct AST *command, struct AST *redirection)
{
    if (!command->value->command->redirection)
    {
        command->value->command->redirection = redirection;
        return;
    }
    add_redirection_next(command->value->command->redirection, redirection);
}
