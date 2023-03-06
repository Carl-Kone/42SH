#ifndef VARIABLE_LIST_H
#define VARIABLE_LIST_H

struct variable_list
{
    char *name;
    char *value;
    struct variable_list *next;
};

struct variable_list *variable_list_init(void);
void variable_list_free(struct variable_list *variable_list);
void variable_list_assign(struct variable_list *variable_list, char *name,
                          char *value);
char *variable_list_get(struct variable_list *variable_list, char *name);

#endif /* !VARIABLE_LIST_H */
