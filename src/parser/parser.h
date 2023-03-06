#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer/lexer.h"
#include "../lexer/token.h"
#include "ast.h"

struct AST *parse(struct lexer *lexer, bool is_in_input_mode);

struct parser *parser_init(struct lexer *lexer, bool is_in_input_mode);

bool parser_all_input_parsed(struct parser *parser);

struct AST *parser_parse(struct parser *parser);
void parser_free(struct parser *parser);

#endif /* ! PARSER_H */
