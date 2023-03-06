#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lexer/lexer.h"
#include "../lexer/token.h"
#include "ast.h"

struct parser
{
    struct lexer *lexer;
    struct token *stream;
    enum filter_token filter_token;
    bool fatal_error;
    bool is_in_prompt_mode;
    bool end_of_file_reached;
    bool preceding_command;
    struct token *look_ahead;
};

bool rule_if(struct parser *parser, struct AST *root);
bool and_or(struct parser *parser, struct AST *root);
bool rule_elif(struct parser *parser, struct AST **root);

bool eat(struct parser *parser, bool (*predicate)(struct token *token));

bool is_newline_or_eof(struct parser *parser);

bool compound_list_condition(struct parser *parser, struct AST *compound_list);

struct token *parser_next_token(struct parser *parser);

bool eat_newlines(struct parser *parser);

enum and_or_type get_and_or_type(struct parser *parser);

void look_ahead(struct parser *parser);

struct lexer *lexer_copy(struct parser *parser);

#endif /* ! UTILS_H */
