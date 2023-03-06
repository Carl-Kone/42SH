#include "variable_list.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void variable_list_free(struct variable_list *variable_list)
{
    // Free the memory used by the current variable_list struct
    free(variable_list->name);
    free(variable_list->value);

    if (variable_list->next)
        variable_list_free(variable_list->next);

    free(variable_list);
}

void variable_list_assign(struct variable_list *variable_list, char *name,
                          char *value)
{
    // Check if the variable already exists in the list
    struct variable_list *current = variable_list;
    while (current->next)
    {
        if (current->name && strcmp(current->name, name) == 0)
        {
            // If the variable already exists, update its value
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

    // If the variable does not exist, create a new element in the list
    current->next =
        (struct variable_list *)malloc(sizeof(struct variable_list));
    current->next->name = name;
    current->next->value = value;
    current->next->next = NULL;
}

static char *get_pwd(void)
{
    char *pwd = getcwd(NULL, 0);
    return pwd;
}

static char *get_uid(void)
{
    // max UID is on 5 bytes
    char *uid = calloc(1, 6);
    sprintf(uid, "%d", getuid());
    return uid;
}

static char *get_random(void)
{
    size_t rand_max = 64000;
    char *random = calloc(1, 5 + 1);
    sprintf(random, "%d", (int)rand() % (int)(rand_max + 1 - 0));
    return random;
}

char *variable_list_get(struct variable_list *variable_list, char *name)
{
    assert(name);
    variable_list_assign(variable_list, strdup("RANDOM"), get_random());
    // Search for the variable in the list
    struct variable_list *current = variable_list;
    while (current)
    {
        if (current->name && strcmp(current->name, name) == 0)
        {
            // If the variable is found, return its value
            return current->value ? strdup(current->value) : NULL;
        }
        current = current->next;
    }

    return NULL;
}

struct variable_list *variable_list_init(void)
{
    // Allocate memory for a new variable_list struct and set next to NULL
    struct variable_list *new_list =
        (struct variable_list *)malloc(sizeof(struct variable_list));
    new_list->name = NULL;
    new_list->value = NULL;
    new_list->next = NULL;
    variable_list_assign(new_list, strdup("PWD"), get_pwd());
    variable_list_assign(new_list, strdup("OLDPWD"), get_pwd());
    variable_list_assign(new_list, strdup("UID"), get_uid());
    return new_list;
}
