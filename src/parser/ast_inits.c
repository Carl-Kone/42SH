#include <stdlib.h>
#include <string.h>

#include "ast.h"

static struct AST_input *input_init(void)
{
    return calloc(1, sizeof(struct AST_input));
}

// word needs to be dupped beforehand
static struct AST_element *element_init(char *word)
{
    struct AST_element *el = calloc(1, sizeof(struct AST_element));
    if (word)
        el->word = strdup(word);
    return el;
}

static struct AST_simple_command *simple_command_init(char *command_name)
{
    struct AST_simple_command *sc =
        calloc(1, sizeof(struct AST_simple_command));

    if (!command_name)
        return sc;

    sc->command_name = strdup(command_name);
    return sc;
}

static struct AST_pipeline *pipeline_init(void)
{
    return calloc(1, sizeof(struct AST_pipeline));
}

static struct AST_command *command_init(void)
{
    return calloc(1, sizeof(struct AST_command));
}

static struct AST_and_or *and_or_init(void)
{
    return calloc(1, sizeof(struct AST_and_or));
}

static struct AST_list *list_init(void)
{
    return calloc(1, sizeof(struct AST_list));
}

static struct AST_if *if_init(void)
{
    return calloc(1, sizeof(struct AST_if));
}

static struct AST_else *else_init(void)
{
    return calloc(1, sizeof(struct AST_else));
}

static struct AST_shell_command *shell_command_init(void)
{
    return calloc(1, sizeof(struct AST_shell_command));
}

static struct AST_compound_list *compound_list_init(void)
{
    return calloc(1, sizeof(struct AST_compound_list));
}

static struct AST_prefix *prefix_init(void)
{
    return calloc(1, sizeof(struct AST_prefix));
}

static enum REDIRECTION_TYPE redirection_get_type(enum token_type type)
{
    switch (type)
    {
    case OP_LESS:
        return REDIRECTING_INPUT;
    case OP_GREAT:
        return REDIRECTING_OUTPUT;
    case OP_DGREAT:
        return APPEND_OUTPUT;
    case OP_GREATAND:
        return DUPLICATE_OUTPUT;
    case OP_LESSAND:
        return DUPLICATE_INPUT;
    case OP_LESSGREAT:
        return OPEN_FD;
    case OP_CLOBBER:
        return CLOBBER;
    default:
        return ERROR;
    }
}

bool redirection_set_type(struct AST *red, enum token_type op)
{
    enum REDIRECTION_TYPE type = redirection_get_type(op);
    red->value->redirection->type = type;
    return type != ERROR;
}

static struct AST_redirection *redirection_init(void)
{
    struct AST_redirection *red = calloc(1, sizeof(struct AST_redirection));
    red->io_number = -1;
    red->type = ERROR;
    return red;
}

static struct AST_rule_while *rule_while_init(void)
{
    return calloc(1, sizeof(struct AST_rule_while));
}

static struct AST_rule_until *rule_until_init(void)
{
    return calloc(1, sizeof(struct AST_rule_until));
}

static struct AST_rule_for *rule_for_init(char *value)
{
    struct AST_rule_for *rule_for = calloc(1, sizeof(struct AST_rule_for));
    rule_for->word = strdup(value);
    return rule_for;
}

struct word_list *word_list_init(char *word)
{
    struct word_list *word_list = calloc(1, sizeof(struct word_list));
    word_list->word = strdup(word);
    return word_list;
}

static struct AST_funcdec *funcdec_init(char *value)
{
    struct AST_funcdec *funcdec = calloc(1, sizeof(struct AST_funcdec));
    funcdec->name = strdup(value);
    return funcdec;
}

static struct AST_rule_case *rule_case_init(char *value)
{
    struct AST_rule_case *rule_case = calloc(1, sizeof(struct AST_rule_case));
    rule_case->word = strdup(value);
    return rule_case;
}

static struct AST_case_clause *case_clause_init(void)
{
    return calloc(1, sizeof(struct AST_case_clause));
}

static struct AST_case_item *case_item_init(void)
{
    return calloc(1, sizeof(struct AST_case_item));
}

struct AST *ast_init_2(enum AST_type type, char *value, struct AST *ast)
{
    switch (type)
    {
    case SHELL_COMMAND:
        ast->value->shell_command = shell_command_init();
        break;
    case COMPOUND_LIST:
        ast->value->compound_list = compound_list_init();
        break;
    case REDIRECTION:
        ast->value->redirection = redirection_init();
        break;
    case PREFIX:
        ast->value->prefix = prefix_init();
        break;
    case WHILE:
        ast->value->rule_while = rule_while_init();
        break;
    case UNTIL:
        ast->value->rule_until = rule_until_init();
        break;
    case FOR:
        ast->value->rule_for = rule_for_init(value);
        break;
    case FUNCTION:
        ast->value->funcdec = funcdec_init(value);
        break;
    case RULE_CASE:
        ast->value->rule_case = rule_case_init(value);
        break;
    case CASE_CLAUSE:
        ast->value->case_clause = case_clause_init();
        break;
    case CASE_ITEM:
        ast->value->case_item = case_item_init();
        break;
    default:
        printf("not yet implemented!\n");
    }
    return ast;
}

struct AST *ast_init(enum AST_type type, char *value)
{
    struct AST *ast = calloc(1, sizeof(struct AST));
    ast->type = type;
    ast->value = calloc(1, sizeof(union AST_value));
    switch (type)
    {
    case INPUT:
        ast->value->input = input_init();
        break;
    case ELEMENT:
        ast->value->element = element_init(value);
        break;
    case SIMPLE_COMMAND:
        ast->value->simple_command = simple_command_init(value);
        break;
    case PIPELINE:
        ast->value->pipeline = pipeline_init();
        break;
    case COMMAND:
        ast->value->command = command_init();
        break;
    case AND_OR:
        ast->value->and_or = and_or_init();
        break;
    case LIST:
        ast->value->list = list_init();
        break;
    case IF:
        ast->value->rule_if = if_init();
        break;
    case ELSE:
        ast->value->else_clause = else_init();
        break;
    default:
        return ast_init_2(type, value, ast);
    }
    return ast;
}
