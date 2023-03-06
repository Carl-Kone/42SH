#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>
#include <stddef.h>

enum token_type
{
    TOKEN_WORD,
    TOKEN_EOF,
    TOKEN_NEWLINE,
    RESERVED_IF,
    RESERVED_FI,
    RESERVED_ELIF,
    RESERVED_ELSE,
    RESERVED_THEN,
    OP_SEMI,
    OP_LESS,
    OP_GREAT,
    OP_DGREAT,
    OP_LESSAND,
    OP_GREATAND,
    OP_LESSGREAT,
    OP_CLOBBER,
    IO_NUMBER,
    OP_PIPE,
    TOKEN_ASSIGNMENT_WORD,
    RESERVED_NEGATE,
    RESERVED_WHILE,
    RESERVED_UNTIL,
    OP_ANDAND,
    OP_OROR,
    RESERVED_FOR,
    RESERVED_DO,
    RESERVED_DONE,
    RESERVED_IN,
    OP_LEFT_PAREN,
    OP_RIGHT_PAREN,
    RESERVED_LEFT_CURLY,
    RESERVED_RIGHT_CURLY,
    RESERVED_CASE,
    RESERVED_ESAC,
    OP_SEMI_SEMI,
};

union token_value
{
    char *word;
    int io_number;
};

struct token
{
    enum token_type type;
    union token_value value;
    bool is_quoted;
    size_t first_quote_index;
};

// Token creation.
struct token *token_eof(void);
struct token *token_operator(char *op);
struct token *token_word(char *word);
struct token *token_io_number(char *io_number);

// Token deletion
void token_free(struct token *token);

// Token conversion.
/// Create a new token with the input token.
/// The input token is not freed.
/// The output token must be freed after usage.
/// If it is a word and can be converted to a reserved word, it will be.
/// It it is a word and cannot be converted, a word is returned.
/// If it is not a word, it fails.
struct token *token_to_reserved(struct token *token);

/// Create a new token with the input token.
/// The input token is not freed.
/// The output token must be freed after usage.
/// If it is a word and can be converted to an assignment word, it will be.
/// If it is a word and cannot be converted, a word is returned.
/// If it is not a word, it fails.
struct token *token_to_assignment(struct token *token,
                                  bool has_quoted_or_escaped,
                                  size_t first_quote_or_escape_idx);

// Token usage.
bool token_is_word(struct token *token);
bool token_is_semi_colon(struct token *token);
bool token_is_eof(struct token *token);
bool token_is_reserved_if(struct token *token);
bool token_is_reserved_then(struct token *token);
bool token_is_reserved_else(struct token *token);
bool token_is_reserved_fi(struct token *token);
bool token_is_reserved_elif(struct token *token);
bool token_is_newline(struct token *token);
bool token_is_less(struct token *token);
bool token_is_great(struct token *token);
bool token_is_dgreat(struct token *token);
bool token_is_less_and(struct token *token);
bool token_is_great_and(struct token *token);
bool token_is_less_great(struct token *token);
bool token_is_clobber(struct token *token);
bool token_is_redirection_operator(struct token *token);
bool token_is_io_number(struct token *token);
bool token_is_pipe(struct token *token);
bool token_is_assignment_word(struct token *token);
bool token_is_reserved_negate(struct token *token);
bool token_is_reserved_while(struct token *token);
bool token_is_reserved_until(struct token *token);
bool token_is_andand(struct token *token);
bool token_is_oror(struct token *token);
bool token_is_reserved_for(struct token *token);
bool token_is_reserved_do(struct token *token);
bool token_is_reserved_done(struct token *token);
bool token_is_reserved_in(struct token *token);
bool token_is_left_paren(struct token *token);
bool token_is_right_paren(struct token *token);
bool token_is_left_curly(struct token *token);
bool token_is_right_curly(struct token *token);
bool token_is_semi_semi(struct token *token);
bool token_is_case(struct token *token);
bool token_is_esac(struct token *token);

// Return the word in a word token.
// The string is a reference to an allocated string, no need to free it.
// If the token is not a word, NULL is returned.
char *token_get_word_value(struct token *token);
char *token_get_assignment_word_value(struct token *token);
int token_get_io_number(struct token *token);
bool token_is_io_number(struct token *token);

// Token debug.
void token_print(struct token *token);

#endif /* !TOKEN_H */
