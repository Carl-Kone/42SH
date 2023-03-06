#include "parser.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "string.h"
#include "utils.h"

// redirection = [IONUMBER] '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' WORD ;
// root is either an element or a command
static bool redirection(struct parser *parser, struct AST *root)
{
    int io_number = -1;
    if (token_is_io_number(parser->stream))
        io_number = token_get_io_number(parser->stream);

    eat(parser, token_is_io_number);

    struct AST *redirection = ast_init(REDIRECTION, NULL);

    if (!redirection_set_type(redirection, parser->stream->type))
        goto error;

    eat(parser, token_is_redirection_operator);

    if (!token_is_word(parser->stream))
    {
        parser->fatal_error = true;
        goto error;
    }

    redirection->value->redirection->io_number = io_number;
    redirection->value->redirection->word =
        strdup(token_get_word_value(parser->stream));

    ast_add_redirection(root, redirection);

    // always true just lazy to do 2 lines for that
    return eat(parser, token_is_word);

error:
    ast_free(redirection);
    return false;
}

// element =
//      WORD
//      | redirection
//      ;
// If not mistaken the root is always a simple command
static bool element(struct parser *parser, struct AST *root)
{
    struct AST *element = ast_init(ELEMENT, NULL);
    if (redirection(parser, element))
    {
        simple_command_add_element(root, element);
        return true;
    }

    if (!token_is_word(parser->stream))
    {
        ast_free(element);
        return false;
    }

    // is a word so we add it to simple command
    element->value->element->word =
        strdup(token_get_word_value(parser->stream));

    eat(parser, token_is_word);

    simple_command_add_element(root, element);
    return true;
}

// root should be a simple command
// prefix = redirection ;
bool prefix(struct parser *parser, struct AST *root)
{
    struct AST *prefix = ast_init(PREFIX, NULL);
    if (token_is_assignment_word(parser->stream))
    {
        prefix->value->prefix->assignment_word =
            strdup(token_get_assignment_word_value(parser->stream));
        eat(parser, token_is_assignment_word);
        simple_command_add_prefix(root, prefix);
        return true;
    }

    if (redirection(parser, prefix))
    {
        simple_command_add_prefix(root, prefix);
        return true;
    }

    ast_free(prefix);
    return false;
}

// simple_command =
//      prefix { prefix }
//      | { prefix } WORD { element }
//      ;
// root should be a command
static bool simple_command(struct parser *parser, struct AST *root)
{
    struct lexer *lexer = lexer_copy(parser);
    struct token *token = next_token(lexer, FILTER_WORD);
    if (token_is_left_paren(token))
    {
        token_free(token);
        lexer_merge(parser->lexer, lexer);
        return false;
    }
    token_free(token);
    lexer_merge(parser->lexer, lexer);

    struct AST *sc = ast_init(SIMPLE_COMMAND, NULL);

    while (prefix(parser, sc))
        continue;

    // if token is not word, we are in rule 1 and need to check that we have
    // prefixes or else it is invalid
    bool is_word = token_is_word(parser->stream);
    if (!is_word && sc->value->simple_command->prefix == NULL)
        goto error;
    else if (!is_word)
    {
        root->value->command->simple_or_shell_command = sc;
        return true;
    }

    sc->value->simple_command->command_name =
        strdup(token_get_word_value(parser->stream));

    // ELEMENTS
    parser->filter_token = FILTER_WORD;

    eat(parser, token_is_word);

    while (element(parser, sc))
        continue;

    parser->filter_token = FILTER_RESERVED_WORD | FILTER_WORD;

    root->value->command->simple_or_shell_command = sc;

    return true;

error:
    ast_free(sc);
    return false;
}
// compound_list = {'\n'} and_or { ';' | '\n' {'\n'} and_or } [';'] {'\n'} ;
// root should be a rule_if or else_clause
static bool compound_list(struct parser *parser, struct AST **compound_list_ptr)
{
    struct AST *compound_list = ast_init(COMPOUND_LIST, NULL);

    while (eat(parser, token_is_newline))
        continue;

    if (!and_or(parser, compound_list))
        goto error;

    while (compound_list_condition(parser, compound_list))
        continue;

    eat(parser, token_is_semi_colon);

    while (eat(parser, token_is_newline))
        continue;

    *compound_list_ptr = compound_list;

    return true;

error:
    ast_free(compound_list);
    return false;
}

// else_clause =
//      'else' compound_list
//      | 'elif' compound_list 'then' compound_list [else_clause]
//      ;
// as rule_if and else_clause depend on each other, this first declaration
// is needed
static bool else_clause(struct parser *parser, struct AST *root)
{
    if (!token_is_reserved_elif(parser->stream)
        && !token_is_reserved_else(parser->stream))
        return false;

    bool is_else = token_is_reserved_else(parser->stream);

    // if not else -> elif (so IF)
    struct AST *else_clause = ast_init(is_else ? ELSE : IF, NULL);

    // has to be either else or elif
    if (is_else)
    {
        eat(parser, token_is_reserved_else);
        // tries to add a compound_list to else_clause
        if (!compound_list(parser,
                           &(else_clause->value->else_clause->compound_list)))
            goto error;
    }
    else if (token_is_reserved_elif(parser->stream))
    {
        // tries to add a rule_if to else_clause (elif)
        if (!rule_elif(parser, &else_clause))
            goto error;
    }
    else
        goto error;

    root->value->rule_if->else_clause = else_clause;
    return true;

error:
    ast_free(else_clause);
    parser->fatal_error = true;
    return false;
}

bool rule_elif(struct parser *parser, struct AST **root)
{
    if (!eat(parser, token_is_reserved_elif))
        return false;

    struct AST *rule_if = *root;

    // adds both compound_list to rule_if tree, frees and returns false if
    // fails
    if (!compound_list(parser, &(rule_if->value->rule_if->if_compound)))
        goto error;

    if (!eat(parser, token_is_reserved_then)
        || !compound_list(parser, &(rule_if->value->rule_if->then_compound)))
        goto error;

    // not mandatory for an if to have an else, we don't care about the
    // return value
    else_clause(parser, rule_if);

    return true;

error:
    ast_free(rule_if);
    parser->fatal_error = true;
    return false;
}
// rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi'
// root should be a shell_command
bool rule_if(struct parser *parser, struct AST *root)
{
    if (!eat(parser, token_is_reserved_if))
        return false;

    struct AST *rule_if = ast_init(IF, NULL);

    // adds both compound_list to rule_if tree, frees and returns false if
    // fails
    if (!compound_list(parser, &(rule_if->value->rule_if->if_compound)))
        goto error;

    if (!eat(parser, token_is_reserved_then)
        || !compound_list(parser, &(rule_if->value->rule_if->then_compound)))
        goto error;

    // not mandatory for an if to have an else, we don't care about the
    // return value
    else_clause(parser, rule_if);

    if (!eat(parser, token_is_reserved_fi))
        goto error;

    // depending on if we are dealing with an if or elif, we want to add it
    // to different things
    root->value->shell_command->rule_if = rule_if;

    return true;

error:
    ast_free(rule_if);
    parser->fatal_error = true;
    return false;
}

// rule_while = 'while' compound_list 'do' compound_list 'done' ;
// root should be a shell_command
static bool rule_while(struct parser *parser, struct AST *root)
{
    if (!eat(parser, token_is_reserved_while))
        return false;

    struct AST *rule_while = ast_init(WHILE, NULL);

    if (!compound_list(parser, &(rule_while->value->rule_while->condition))
        || !eat(parser, token_is_reserved_do)
        || !compound_list(parser, &(rule_while->value->rule_while->to_execute))
        || !eat(parser, token_is_reserved_done))
        goto error;

    root->value->shell_command->rule_while = rule_while;
    return true;

error:
    ast_free(rule_while);
    parser->fatal_error = true;
    return false;
}

// rule_until = 'until' compound_list 'do' compound_list 'done' ;
// root should be a shell_command
static bool rule_until(struct parser *parser, struct AST *root)
{
    if (!eat(parser, token_is_reserved_until))
        return false;

    struct AST *rule_until = ast_init(UNTIL, NULL);

    if (!compound_list(parser, &(rule_until->value->rule_until->condition))
        || !eat(parser, token_is_reserved_do)
        || !compound_list(parser, &(rule_until->value->rule_until->to_execute))
        || !eat(parser, token_is_reserved_done))
        goto error;

    root->value->shell_command->rule_until = rule_until;
    return true;

error:
    ast_free(rule_until);
    parser->fatal_error = true;
    return false;
}
// rule_for = 'for' WORD [';'] | [ {'\n'} 'in' { WORD } ';' | '\n' ] {'\n'} 'do'
// compound_list 'done' ; root should be a shell_command tree;
static bool rule_for(struct parser *parser, struct AST *root)
{
    if (!eat(parser, token_is_reserved_for))
        return false;

    struct AST *rule_for = ast_init(FOR, token_get_word_value(parser->stream));
    if (!token_is_word(parser->stream))
        goto error;

    eat(parser, token_is_word);

    if (!eat(parser, token_is_semi_colon))
    {
        // second part of first |
        eat_newlines(parser);
        if (eat(parser, token_is_reserved_in))
        {
            while (token_is_word(parser->stream))
            {
                if (!rule_for->value->rule_for->word_list)
                    rule_for->value->rule_for->word_list =
                        word_list_init(token_get_word_value(parser->stream));
                else
                    word_list_add(rule_for->value->rule_for->word_list,
                                  token_get_word_value(parser->stream));
                eat(parser, token_is_word);
            }

            if (!eat(parser, token_is_semi_colon)
                && !eat(parser, token_is_newline))
                goto error;
        }
    }
    eat_newlines(parser);
    if (!eat(parser, token_is_reserved_do)
        || !compound_list(parser, &(rule_for->value->rule_for->to_execute))
        || !eat(parser, token_is_reserved_done))
        goto error;

    root->value->shell_command->rule_for = rule_for;
    return true;

error:
    ast_free(rule_for);
    parser->fatal_error = true;
    return false;
}

// ['('] WORD { '|' WORD } ')' {'\n'} [compound_list] ;
// root should be a case_clause
static bool case_item(struct parser *parser, struct AST *root)
{
    struct AST *case_item = NULL;
    if (eat(parser, token_is_left_paren) && !token_is_word(parser->stream))
        goto error;

    if (!token_is_word(parser->stream))
        return false;

    case_item = ast_init(CASE_ITEM, NULL);

    case_item->value->case_item->word_list =
        word_list_init(token_get_word_value(parser->stream));

    eat(parser, token_is_word);
    while (eat(parser, token_is_pipe))
    {
        if (!token_is_word(parser->stream))
            goto error;
        word_list_add(case_item->value->case_item->word_list,
                      token_get_word_value(parser->stream));
        eat(parser, token_is_word);
    }

    if (!eat(parser, token_is_right_paren))
        goto error;
    eat_newlines(parser);
    compound_list(parser, &(case_item->value->case_item->compound_list));
    case_clause_add_case_item(root, case_item);

    return true;
error:
    parser->fatal_error = true;
    ast_free(case_item);
    return false;
}

// case_clause = case_item { ';;' {'\n'} case_item } [';;'] {'\n'} ;
// root should be a rule_case
static bool case_clause(struct parser *parser, struct AST *root)
{
    struct AST *case_clause = ast_init(CASE_CLAUSE, NULL);
    if (!case_item(parser, case_clause))
        goto error;

    while (eat(parser, token_is_semi_semi))
    {
        eat_newlines(parser);
        // already ate ';;' and newlines
        if (!case_item(parser, case_clause))
            goto finished;
    }

    eat_newlines(parser);
finished:
    root->value->rule_case->case_clause = case_clause;
    return true;

error:
    ast_free(case_clause);
    return false;
}

// rule_case = 'case' WORD {'\n'} 'in' {'\n'} [case_clause] 'esac' ;
// root should be a shell_command
static bool rule_case(struct parser *parser, struct AST *root)
{
    if (!eat(parser, token_is_case))
        return false;

    struct AST *rule_case = NULL;

    if (!token_is_word(parser->stream))
        goto error;

    rule_case = ast_init(RULE_CASE, token_get_word_value(parser->stream));

    eat(parser, token_is_word);

    eat_newlines(parser);

    if (!eat(parser, token_is_reserved_in))
        goto error;

    eat_newlines(parser);

    case_clause(parser, rule_case);

    if (!eat(parser, token_is_esac))
        goto error;

    root->value->shell_command->rule_case = rule_case;

    return true;

error:
    parser->fatal_error = true;
    ast_free(rule_case);
    return false;
}

// shell_command =
//      '{' compound_list '}'
//      | '(' compound_list ')'
//      | rule_if
//      | rule_while
//      | rule_until
//      | rule_case
//      | rule_for
//      ;
// root could be a command or a funcdec
static bool shell_command(struct parser *parser, struct AST *root)
{
    struct AST *shell_command = ast_init(SHELL_COMMAND, NULL);

    if (eat(parser, token_is_left_paren))
    {
        if (!compound_list(parser,
                           &(shell_command->value->shell_command->subshell))
            || !eat(parser, token_is_right_paren))
        {
            parser->fatal_error = true;
            goto error;
        }
    }
    else if (eat(parser, token_is_left_curly))
    {
        if (!compound_list(
                parser, &(shell_command->value->shell_command->command_block))
            || !eat(parser, token_is_right_curly))
        {
            parser->fatal_error = true;
            goto error;
        }
    }
    else if (!rule_if(parser, shell_command)
             && !rule_while(parser, shell_command)
             && !rule_until(parser, shell_command)
             && !rule_for(parser, shell_command)
             && !rule_case(parser, shell_command))
        goto error;

    if (root->type == COMMAND)
        root->value->command->simple_or_shell_command = shell_command;
    else // is a function
        root->value->funcdec->shell_command = shell_command;

    return true;

error:
    ast_free(shell_command);
    return false;
}

// funcdec = WORD '(' ')' {'\n'} shell_command ;
// root should be a command tree
static bool funcdec(struct parser *parser, struct AST *root)
{
    if (!token_is_word(parser->stream))
        return false;

    struct AST *funcdec =
        ast_init(FUNCTION, token_get_word_value(parser->stream));

    eat(parser, token_is_word);
    if (!eat(parser, token_is_left_paren) || !eat(parser, token_is_right_paren)
        || !eat_newlines(parser) || !shell_command(parser, funcdec))
        goto error;

    root->value->command->funcdec = funcdec;
    return true;

error:
    ast_free(funcdec);
    parser->fatal_error = true;
    return false;
}

// command =
//      simple_command
//      | shell_command { redirection }
//      | funcdec { redirection }
//      ;
// root should be a pipeline
static bool command(struct parser *parser, struct AST *root)
{
    parser->preceding_command = false;

    struct AST *command = ast_init(COMMAND, NULL);
    if (simple_command(parser, command))
        goto add_next_command;

    if (shell_command(parser, command))
    {
        while (redirection(parser, command))
            continue;

        goto add_next_command;
    }
    if (funcdec(parser, command))
    {
        while (redirection(parser, command))
            continue;
        goto add_next_command;
    }

    ast_free(command);
    parser->preceding_command = true;
    return false;

add_next_command:
    command_add_next(root, command);
    parser->preceding_command = true;
    return true;
}

// pipeline = ['!'] command { '|' {'\n'} command } ;
// root should be an and_or
static bool pipeline(struct parser *parser, struct AST *root,
                     enum and_or_type and_or_type)
{
    struct AST *pipeline = ast_init(PIPELINE, NULL);
    pipeline->value->pipeline->and_or_type = and_or_type;
    if (eat(parser, token_is_reserved_negate))
        pipeline->value->pipeline->negation = true;
    if (!command(parser, pipeline))
        goto error;

    while (eat(parser, token_is_pipe))
    {
        if (!(eat_newlines(parser) && command(parser, pipeline)))
        {
            parser->fatal_error = true;
            goto error;
        }
    }

    pipeline_add_next(root, pipeline);
    return true;

error:
    ast_free(pipeline);
    return false;
}

// and_or = pipeline { '&&' | '||' {'\n'} pipeline } ;
// root should be a list
bool and_or(struct parser *parser, struct AST *root)
{
    struct AST *and_or = ast_init(AND_OR, NULL);

    if (!pipeline(parser, and_or, NOTHING))
    {
        ast_free(and_or);
        return false;
    }

    enum and_or_type and_or_type = NOTHING;

    while ((and_or_type = get_and_or_type(parser)) != NOTHING)
    {
        eat_newlines(parser);
        if (!pipeline(parser, and_or, and_or_type))
        {
            ast_free(and_or);
            parser->fatal_error = true;
            return false;
        }
    }
    ast_add_and_or_next(root, and_or);
    return true;
}

// list = and_or { ';' and_or } [ ';' ] ;
// root should be an input
static bool list(struct parser *parser, struct AST *root)
{
    struct AST *list = ast_init(LIST, NULL);
    if (!and_or(parser, list))
    {
        ast_free(list);
        return false;
    }

    while (eat(parser, token_is_semi_colon) && and_or(parser, list))
        continue;

    root->value->input->list = list;
    return true;
}

// input =
//      list '\n'
//      | list EOF
//      | '\n'
//      | EOF
//      ;
static struct AST *input(struct parser *parser)
{
    struct AST *root = ast_init(INPUT, NULL);

    if (list(parser, root) && is_newline_or_eof(parser))
        return root;

    if (is_newline_or_eof(parser))
        return root;
    else
    {
        ast_free(root);
        return NULL;
    }
}

struct parser *parser_init(struct lexer *lexer, bool is_in_input_mode)
{
    struct parser *parser = calloc(1, sizeof(struct parser));
    parser->preceding_command = true;
    parser->filter_token = FILTER_RESERVED_WORD | FILTER_WORD;
    parser->lexer = lexer;
    parser->is_in_prompt_mode = is_in_input_mode;
    parser->fatal_error = false;
    parser->end_of_file_reached = false;
    parser->stream = parser_next_token(parser);

    return parser;
}

bool parser_all_input_parsed(struct parser *parser)
{
    return parser->end_of_file_reached;
}

struct AST *parser_parse(struct parser *parser)
{
    eat(parser, token_is_newline);
    parser->preceding_command = true;
    struct AST *ast = input(parser);
    if (parser->fatal_error)
    {
        ast_free(ast);
        ast = NULL;
    }

    return ast;
}

void parser_free(struct parser *parser)
{
    lexer_free(parser->lexer);
    token_free(parser->stream);
    free(parser);
}
