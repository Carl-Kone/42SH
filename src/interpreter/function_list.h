#ifndef FUNCTION_LIST_H
#define FUNCTION_LIST_H

#include "../parser/ast.h"

struct function_list
{
    char *name;
    struct AST *value;
    struct function_list *next;
};

struct function_list *function_list_init(void);
void function_list_free(struct function_list *function_list);
void function_list_assign(struct function_list *function_list, char *name,
                          struct AST *value);
struct AST *function_list_get(struct function_list *function_list, char *name);

#endif /* !VARIABLE_LIST_H */
