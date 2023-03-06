#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>

#include "cursor.h"
#include "token.h"

enum filter_token
{
    FILTER_WORD = 1,
    FILTER_RESERVED_WORD = 2,
    FILTER_ASSIGNMENT_WORD = 4,
};

struct lexer
{
    /// Cursor used to move on the text
    /// tto be lexed.
    struct cursor *cursor;
    bool io_here_recognized;

    /// Lexeme of the current token being built.
    char *current_token;

    /// Token to return on the next call to
    /// `next_token`
    struct token *token;

    /// Tells the lexer working on a operator.
    bool previous_part_of_op;

    /// Tells the lexer working on a word.
    bool previous_part_of_word;

    /// Internal state used to handle quoting.
    /// single_quoted be reset when the matching
    /// quote is found.
    bool single_quoted;

    /// Internal state used to handle quoting.
    /// double_quoted should be reset when the matching
    /// quote is found.
    bool double_quoted;

    /// Internal state used to handle escaping.
    /// escaped should be reset after the next char
    /// has be escaped.
    bool escaped;

    /// True is the current word being processed
    /// is escaped, single_quoted or double_quoted
    /// Should be set to false before a new token is being
    /// processed.
    bool word_is_quoted;

    /// The index of the first
    /// backslash, single_quote or double_quote
    /// that occured in a word.
    /// Make sens only when `word_is_quoted` is true.
    size_t first_quote_index;

    /// Indicate if the lexer should perform expansions.
    /// In expansion mode, it removes the quotes, the backslashes
    /// and expand variables.
    bool is_in_expansion_mode;

    /// Environment used to perform expansions.
    struct environment *environment;

    /// Tells if the lexer is currently reading a var name.
    /// If it is, calls to `add_char_to_current_token` should
    /// add the char to the var name buffer.
    bool reading_var_name;

    /// Current var name being built.
    char *current_var_name;

    bool is_prompt;

    bool input_end_reached;
};

/// Initialise and returns a pointer
/// to a lexer.
/// Must be freed after usage.
struct lexer *lexer_init(char *input);

/// Free a malloc allocated lexer.
void lexer_free(struct lexer *lexer);

/// Get the next token.
/// It must be freed after usage.
struct token *next_token(struct lexer *lexer, enum filter_token);

void lexer_merge(struct lexer *original, struct lexer *copy);

#endif /* !LEXER_H */
