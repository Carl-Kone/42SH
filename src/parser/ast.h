#ifndef AST_H
#define AST_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer/token.h"

enum AST_type
{
    INPUT,
    LIST,
    SIMPLE_COMMAND,
    ELEMENT,
    AND_OR,
    PIPELINE,
    COMMAND,
    IF,
    ELSE,
    ELIF,
    SHELL_COMMAND,
    COMPOUND_LIST,
    REDIRECTION,
    PREFIX,
    WHILE,
    UNTIL,
    FOR,
    FUNCTION,
    RULE_CASE,
    CASE_CLAUSE,
    CASE_ITEM,
};

union AST_value
{
    struct AST_input *input;
    struct AST_and_or *and_or;
    struct AST_element *element;
    struct AST_simple_command *simple_command;
    struct AST_command *command;
    struct AST_pipeline *pipeline;
    struct AST_list *list;
    struct AST_if *rule_if;
    struct AST_else *else_clause;
    struct AST_shell_command *shell_command;
    struct AST_compound_list *compound_list;
    struct AST_prefix *prefix;
    struct AST_redirection *redirection;
    struct AST_rule_while *rule_while;
    struct AST_rule_until *rule_until;
    struct AST_rule_for *rule_for;
    struct AST_funcdec *funcdec;
    struct AST_rule_case *rule_case;
    struct AST_case_clause *case_clause;
    struct AST_case_item *case_item;
};

struct AST_case_item
{
    struct word_list *word_list;
    struct AST *compound_list;
    struct AST *next;
};

struct AST_case_clause
{
    struct AST *case_item;
};

struct AST_rule_case
{
    char *word;
    struct AST *case_clause;
};

struct AST_input
{
    struct AST *list;
};

// is_word-> false if element is a redirection
struct AST_element
{
    bool is_word;
    char *word;
    struct AST *next;
    struct AST *redirection;
};

// element can be NULL
struct AST_simple_command
{
    char *command_name;
    struct AST *element;
    struct AST *prefix;
};

struct AST_command
{
    struct AST *simple_or_shell_command;
    struct AST *redirection;
    struct AST *next_command;
    struct AST *funcdec;
};

struct AST_funcdec
{
    char *name;
    struct AST *shell_command;
};

enum and_or_type
{
    AND,
    OR,
    NOTHING
};

struct AST_pipeline
{
    struct AST *command;
    bool negation;
    struct AST *next_pipeline;
    enum and_or_type and_or_type;
};

struct AST_and_or
{
    struct AST *pipeline;
    struct AST *next;
};

struct AST_list
{
    struct AST *and_or;
};

struct AST_compound_list
{
    struct AST *and_or;
};

struct AST_else
{
    struct AST *compound_list;
};

struct AST_shell_command
{
    struct AST *command_block;
    struct AST *subshell;
    struct AST *rule_if;
    struct AST *rule_while;
    struct AST *rule_until;
    struct AST *rule_case;
    struct AST *rule_for;
};

/* if_compound->if condition
 then_compound->what to do if "if_compound" is true
 else_clause->can be an else or elif
 AST_if is also used to represent elifs */
struct AST_if
{
    struct AST *if_compound;
    struct AST *then_compound;
    struct AST *else_clause;
};

enum REDIRECTION_TYPE
{
    ERROR,
    REDIRECTING_INPUT,
    CLOBBER,
    APPEND_OUTPUT,
    DUPLICATE_INPUT,
    DUPLICATE_OUTPUT,
    OPEN_FD,
    REDIRECTING_OUTPUT
};

struct AST_redirection
{
    int io_number;
    char *word;
    enum REDIRECTION_TYPE type;
    struct AST *next;
};

struct AST_prefix
{
    struct AST *redirection;
    struct AST *next;
    char *assignment_word;
};

struct AST_rule_while
{
    struct AST *condition;
    struct AST *to_execute;
};

struct AST_rule_until
{
    struct AST *condition;
    struct AST *to_execute;
};

struct word_list
{
    char *word;
    struct word_list *next;
};

struct AST_rule_for
{
    char *word;
    struct word_list *word_list;
    struct AST *to_execute;
};

struct AST
{
    enum AST_type type;
    union AST_value *value;
};

/*---------------------------------------------------------------------FUNCTIONS----------------------------------------------------------------------------------*/
struct AST *ast_init(enum AST_type type, char *value);

void element_add_next(struct AST *el, struct AST *new_el);
void simple_command_add_element(struct AST *sc, struct AST *el);
void and_or_add_next(struct AST *and_or, struct AST *to_add);
void ast_add_and_or_next(struct AST *ast, struct AST *and_or);
bool redirection_set_type(struct AST *red, enum token_type op);
void command_add_redirection(struct AST *shell_command,
                             struct AST *redirection);
void ast_add_redirection(struct AST *ast, struct AST *redirection);
void simple_command_add_prefix(struct AST *simple_command, struct AST *prefix);
void command_add_next(struct AST *el, struct AST *to_add);
void pipeline_add_next(struct AST *root, struct AST *to_add);
void case_clause_add_case_item(struct AST *case_clause, struct AST *case_item);

void word_list_free(struct word_list *word_list);
struct word_list *word_list_init(char *word);
void word_list_add(struct word_list *word_list, char *word);

void ast_free(struct AST *ast);

void ast_print(struct AST *ast);

#endif /* ! AST_H */
