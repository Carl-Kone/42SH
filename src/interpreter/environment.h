#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "function_list.h"
struct environment
{
    int out;
    int in;
    int err;
    struct variable_list *variable_list;
    struct function_list *function_list;
    bool is_interactive;
};

struct environment *environment_init(void);

void environment_free(struct environment *environment);

void assign_variable(struct environment *environment, char *input);

char *environment_expansion(struct environment *environment, char *input);

#endif /* !ENVIRONMENT_H */
