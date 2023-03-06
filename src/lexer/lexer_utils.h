#ifndef LEXER_UTILS_H
#define LEXER_UTILS_H
#include <stdbool.h>

#include "lexer.h"
#include "token.h"

// Reset some internal state (i.e char escaped)
void advance_char(struct lexer *lexer);

// Check if a char can be used as
// an operator first char.
bool can_be_used_as_op_first_char(char c);

// Check if char c can be use the form an operator
// with thr string op.
// i.e op = ">", c = ">"
// => true (>> is a valid op)
bool can_be_used_to_form_op(char *op, char c);

// Blank char that can be skipped.
bool is_blank_char(char c);

// Add a char to the internal string representation
// of the current token being built.
void lexer_add_char_to_current_token(struct lexer *lexer, char c);

// Add a token to the lexer so it will be returned on the next
// "lexer_next_token" call.
// SOuld not be called twice in a row.
void add_token(struct lexer *lexer, struct token *token);

// Delimit the current token stored
// in the lexer.
// Reset the internal state so it is ready
// to read an other token.
struct token *delimit_current_token(struct lexer *lexer);

void lexer_add_char_to_current_var_name(struct lexer *lexer, char c);

void single_quote_start(struct lexer *lexer);
void single_quote_end(struct lexer *lexer);
void double_quote_start(struct lexer *lexer);
void double_quote_end(struct lexer *lexer);
void escape_start(struct lexer *lexer);
void escape_end(struct lexer *lexer);

#endif /* !LEXER_UTILS_H */
