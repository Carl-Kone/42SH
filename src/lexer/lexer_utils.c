#include "lexer_utils.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "cursor.h"
#include "lexer.h"
#include "string.h"
#include "token.h"

void advance_char(struct lexer *lexer)
{
    escape_end(lexer);
    cursor_advance_char(lexer->cursor);
}

bool can_be_used_as_op_first_char(char c)
{
    return c == ';' || c == '\n' || c == '<' || c == '>' || c == '|' || c == '&'
        || c == '(' || c == ')';
}

bool can_be_used_to_form_op(char *op, char c)
{
    char *op_maybe = malloc(sizeof(char) * strlen(op) + 1 + 1);
    strcpy(op_maybe, op);
    strncat(op_maybe, &c, 1);

    if (strcmp(op_maybe, "<>") == 0 || strcmp(op_maybe, "<&") == 0
        || strcmp(op_maybe, ">&") == 0 || strcmp(op_maybe, ">|") == 0
        || strcmp(op_maybe, ">>") == 0 || strcmp(op_maybe, "&&") == 0
        || strcmp(op_maybe, "||") == 0 || strcmp(op_maybe, ";;") == 0)
    {
        free(op_maybe);
        return true;
    }

    free(op_maybe);
    return false;
}

bool is_blank_char(char c)
{
    return c == ' ' || c == '\t';
}

void lexer_add_char_to_current_token(struct lexer *lexer, char c)
{
    if (!lexer->current_token)
    {
        lexer->current_token = malloc(sizeof(char) * 2);
        lexer->current_token[0] = c;
        lexer->current_token[1] = '\0';
    }
    else
    {
        size_t prev_len = strlen(lexer->current_token);
        lexer->current_token = realloc(lexer->current_token, prev_len + 1 + 1);
        lexer->current_token[prev_len] = c;
        lexer->current_token[prev_len + 1] = '\0';
    }
}

void add_token(struct lexer *lexer, struct token *token)
{
    token->is_quoted = lexer->word_is_quoted;
    lexer->token = token;
}

bool is_numeric(char *str)
{
    for (size_t i = 0; str[i]; i++)
        if (str[i] < '0' || str[i] > '9')
            return false;

    return true;
}

static bool is_redirection_char(char c)
{
    return c == '<' || c == '>';
}

struct token *delimit_current_token(struct lexer *lexer)
{
    struct token *token;
    char delimiter = cursor_current_char(lexer->cursor);

    if (lexer->previous_part_of_op)
        token = token_operator(lexer->current_token);
    else if (lexer->previous_part_of_word)
    {
        bool is_io_number =
            is_numeric(lexer->current_token) && is_redirection_char(delimiter);

        token = is_io_number ? token_io_number(lexer->current_token)
                             : token_word(lexer->current_token);
    }
    else
    {
        printf("Cannot delimit token\n");
        exit(2);
    }

    free(lexer->current_token);
    lexer->current_token = NULL;
    lexer->previous_part_of_op = false;
    lexer->previous_part_of_word = false;
    lexer->reading_var_name = false;

    return token;
}
