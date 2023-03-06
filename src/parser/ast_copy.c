#include "ast_copy.h"

#include <assert.h>
#include <stdio.h>

#include "ast.h"
#include "string.h"

static struct AST *ast_input_copy(struct AST_input *ast_input)
{
    struct AST *input = ast_init(INPUT, NULL);
    input->value->input->list = ast_copy(ast_input->list);
    return input;
}

static struct AST *ast_list_copy(struct AST_list *ast_list)
{
    struct AST *list = ast_init(LIST, NULL);
    list->value->list->and_or = ast_copy(ast_list->and_or);
    return list;
}

static struct AST *
ast_simple_command_copy(struct AST_simple_command *ast_simple_command)
{
    struct AST *simple_command =
        ast_init(SIMPLE_COMMAND, ast_simple_command->command_name);
    simple_command->value->simple_command->element =
        ast_copy(ast_simple_command->element);
    simple_command->value->simple_command->prefix =
        ast_copy(ast_simple_command->prefix);
    return simple_command;
}

static struct AST *ast_element_copy(struct AST_element *ast_element)
{
    struct AST *element = ast_init(ELEMENT, ast_element->word);
    element->value->element->is_word = ast_element->is_word;
    element->value->element->next = ast_copy(ast_element->next);
    element->value->element->redirection = ast_copy(ast_element->redirection);
    return element;
}

static struct AST *ast_and_or_copy(struct AST_and_or *ast_and_or)
{
    struct AST *and_or = ast_init(AND_OR, NULL);
    and_or->value->and_or->next = ast_copy(ast_and_or->next);
    and_or->value->and_or->pipeline = ast_copy(ast_and_or->pipeline);
    return and_or;
}

static struct AST *ast_pipeline_copy(struct AST_pipeline *ast_pipeline)
{
    struct AST *pipeline = ast_init(PIPELINE, NULL);
    pipeline->value->pipeline->command = ast_copy(ast_pipeline->command);
    pipeline->value->pipeline->next_pipeline =
        ast_copy(ast_pipeline->next_pipeline);
    pipeline->value->pipeline->negation = ast_pipeline->negation;
    pipeline->value->pipeline->and_or_type = ast_pipeline->and_or_type;
    return pipeline;
}

static struct AST *ast_command_copy(struct AST_command *ast_command)
{
    struct AST *command = ast_init(COMMAND, NULL);
    command->value->command->next_command = ast_copy(ast_command->next_command);
    command->value->command->simple_or_shell_command =
        ast_copy(ast_command->simple_or_shell_command);
    command->value->command->funcdec = ast_copy(ast_command->funcdec);
    command->value->command->redirection = ast_copy(ast_command->redirection);
    return command;
}

static struct AST *ast_if_copy(struct AST_if *ast_if)
{
    struct AST *_if = ast_init(IF, NULL);
    _if->value->rule_if->else_clause = ast_copy(ast_if->else_clause);
    _if->value->rule_if->if_compound = ast_copy(ast_if->if_compound);
    _if->value->rule_if->then_compound = ast_copy(ast_if->then_compound);
    return _if;
}

static struct AST *ast_else_copy(struct AST_else *ast_else)
{
    struct AST *_else = ast_init(ELSE, NULL);
    _else->value->else_clause->compound_list =
        ast_copy(ast_else->compound_list);
    return _else;
}

static struct AST *
ast_shell_command_copy(struct AST_shell_command *ast_shell_command)
{
    struct AST *shell_command = ast_init(SHELL_COMMAND, NULL);
    shell_command->value->shell_command->command_block =
        ast_copy(ast_shell_command->command_block);
    shell_command->value->shell_command->rule_for =
        ast_copy(ast_shell_command->rule_for);
    shell_command->value->shell_command->rule_if =
        ast_copy(ast_shell_command->rule_if);
    shell_command->value->shell_command->rule_until =
        ast_copy(ast_shell_command->rule_until);
    shell_command->value->shell_command->rule_while =
        ast_copy(ast_shell_command->rule_while);
    shell_command->value->shell_command->subshell =
        ast_copy(ast_shell_command->subshell);
    shell_command->value->shell_command->rule_case =
        ast_copy(ast_shell_command->rule_case);
    return shell_command;
}

static struct AST *
ast_compound_list_copy(struct AST_compound_list *ast_compound_list)
{
    struct AST *compound_list = ast_init(COMPOUND_LIST, NULL);
    compound_list->value->compound_list->and_or =
        ast_copy(ast_compound_list->and_or);
    return compound_list;
}

static struct AST *ast_redirection_copy(struct AST_redirection *ast_redirection)
{
    struct AST *redirection = ast_init(REDIRECTION, ast_redirection->word);
    redirection->value->redirection->io_number = ast_redirection->io_number;
    redirection->value->redirection->type = ast_redirection->type;
    redirection->value->redirection->next = ast_copy(ast_redirection->next);
    return redirection;
}

static struct AST *ast_prefix_copy(struct AST_prefix *ast_prefix)
{
    struct AST *prefix = ast_init(PREFIX, ast_prefix->assignment_word);
    prefix->value->prefix->redirection = ast_copy(ast_prefix->redirection);
    prefix->value->prefix->next = ast_copy(ast_prefix->next);
    return prefix;
}

static struct AST *ast_while_copy(struct AST_rule_while *ast_while)
{
    struct AST *_while = ast_init(WHILE, NULL);
    _while->value->rule_while->condition = ast_copy(ast_while->condition);
    _while->value->rule_while->to_execute = ast_copy(ast_while->to_execute);
    return _while;
}

static struct AST *ast_until_copy(struct AST_rule_until *ast_until)
{
    struct AST *_until = ast_init(UNTIL, NULL);
    _until->value->rule_while->condition = ast_copy(ast_until->condition);
    _until->value->rule_while->to_execute = ast_copy(ast_until->to_execute);
    return _until;
}

static struct word_list *word_list_copy(struct word_list *word_list)
{
    if (!word_list)
        return NULL;

    struct word_list *_new = malloc(sizeof(struct word_list));
    _new->word = strdup(word_list->word);
    _new->next = word_list_copy(word_list->next);
    return _new;
}

static struct AST *ast_for_copy(struct AST_rule_for *ast_for)
{
    struct AST *_for = ast_init(FOR, ast_for->word);
    _for->value->rule_for->word_list = word_list_copy(ast_for->word_list);
    _for->value->rule_for->to_execute = ast_copy(ast_for->to_execute);
    return _for;
}

static struct AST *ast_function_copy(struct AST_funcdec *ast_funcdec)
{
    struct AST *funcdec = ast_init(FUNCTION, ast_funcdec->name);
    funcdec->value->funcdec->shell_command =
        ast_copy(ast_funcdec->shell_command);
    return funcdec;
}

static struct AST *AST_rule_case_copy(struct AST_rule_case *ast_rule_case)
{
    struct AST *rule_case = ast_init(RULE_CASE, ast_rule_case->word);
    rule_case->value->rule_case->case_clause =
        ast_copy(ast_rule_case->case_clause);
    return rule_case;
}

static struct AST *AST_case_clause_copy(struct AST_case_clause *ast_case_clause)
{
    struct AST *case_clause = ast_init(CASE_CLAUSE, NULL);
    case_clause->value->case_clause->case_item =
        ast_copy(ast_case_clause->case_item);
    return case_clause;
}

static struct AST *AST_case_item_copy(struct AST_case_item *ast_case_item)
{
    struct AST *case_item = ast_init(CASE_ITEM, NULL);
    case_item->value->case_item->next = ast_copy(ast_case_item->next);
    case_item->value->case_item->compound_list =
        ast_copy(ast_case_item->compound_list);
    case_item->value->case_item->word_list =
        word_list_copy(ast_case_item->word_list);
    return case_item;
}

static struct AST *ast_copy_2(struct AST *ast)
{
    if (!ast)
        return NULL;
    switch (ast->type)
    {
    case SHELL_COMMAND:
        return ast_shell_command_copy(ast->value->shell_command);
    case COMPOUND_LIST:
        return ast_compound_list_copy(ast->value->compound_list);
    case REDIRECTION:
        return ast_redirection_copy(ast->value->redirection);
    case PREFIX:
        return ast_prefix_copy(ast->value->prefix);
    case WHILE:
        return ast_while_copy(ast->value->rule_while);
    case UNTIL:
        return ast_until_copy(ast->value->rule_until);
    case FOR:
        return ast_for_copy(ast->value->rule_for);
    case FUNCTION:
        return ast_function_copy(ast->value->funcdec);
    case RULE_CASE:
        return AST_rule_case_copy(ast->value->rule_case);
    case CASE_CLAUSE:
        return AST_case_clause_copy(ast->value->case_clause);
    case CASE_ITEM:
        return AST_case_item_copy(ast->value->case_item);
    default:
        fprintf(stderr, "ast cpy error\n");
        exit(2);
    }
}

struct AST *ast_copy(struct AST *ast)
{
    if (!ast)
        return NULL;

    switch (ast->type)
    {
    case INPUT:
        return ast_input_copy(ast->value->input);
    case LIST:
        return ast_list_copy(ast->value->list);
    case SIMPLE_COMMAND:
        return ast_simple_command_copy(ast->value->simple_command);
    case ELEMENT:
        return ast_element_copy(ast->value->element);
    case AND_OR:
        return ast_and_or_copy(ast->value->and_or);
    case PIPELINE:
        return ast_pipeline_copy(ast->value->pipeline);
    case COMMAND:
        return ast_command_copy(ast->value->command);
    case IF:
        return ast_if_copy(ast->value->rule_if);
    case ELSE:
        return ast_else_copy(ast->value->else_clause);
    case ELIF:
        fprintf(stderr, "THEO TOLD ME IT WONT HAPPEN (AST COPY)\n");
        exit(1);
        break;
    default:
        return ast_copy_2(ast);
    }

    assert(false && "never reach here");
    return NULL;
}
