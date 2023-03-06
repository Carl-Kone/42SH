#include "function_list.h"

struct function_list *function_list_init(void)
{
    // Allocate memory for a new function_list struct and set next to NULL
    struct function_list *new_list =
        (struct function_list *)malloc(sizeof(struct function_list));
    new_list->name = NULL;
    new_list->value = NULL;
    new_list->next = NULL;
    return new_list;
}

void function_list_free(struct function_list *function_list)
{
    // Free the memory used by the current function_list struct
    free(function_list->name);
    ast_free(function_list->value);

    if (function_list->next)
        function_list_free(function_list->next);

    free(function_list);
}

void function_list_assign(struct function_list *function_list, char *name,
                          struct AST *value)
{
    // Check if the function already exists in the list
    struct function_list *current = function_list;
    while (current->next)
    {
        if (current->name && strcmp(current->name, name) == 0)
        {
            // If the function already exists, update its value
            free(current->value);
            current->value = value;
            free(name);
            return;
        }
        current = current->next;
    }

    if (current->name && strcmp(current->name, name) == 0)
    {
        free(current->value);
        current->value = value;
        free(name);
        return;
    }

    // If the function does not exist, create a new element in the list
    current->next =
        (struct function_list *)malloc(sizeof(struct function_list));
    current->next->name = name;
    current->next->value = value;
    current->next->next = NULL;
}

struct AST *function_list_get(struct function_list *function_list, char *name)
{
    assert(name);
    // Search for the function in the list
    struct function_list *current = function_list;
    while (current)
    {
        if (current->name && strcmp(current->name, name) == 0)
        {
            // If the function is found, return its value
            // TODO ASTDUP
            return current->value;
        }
        current = current->next;
    }
    // If the function is not found, return NULL
    return NULL;
}
