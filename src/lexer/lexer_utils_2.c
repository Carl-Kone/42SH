#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "cursor.h"
#include "lexer.h"
#include "lexer_utils.h"
#include "string.h"
#include "token.h"

static void quote_word(struct lexer *lexer)
{
    if (!lexer->word_is_quoted)
    {
        if (lexer->current_token)
        {
            size_t current_len = strlen(lexer->current_token);
            lexer->first_quote_index = current_len;
        }
        else
        {
            lexer->first_quote_index = 0;
        }
    }

    lexer->word_is_quoted = true;
    lexer->previous_part_of_word = true;
}

void single_quote_start(struct lexer *lexer)
{
    assert(!lexer->single_quoted);
    lexer->single_quoted = true;
    quote_word(lexer);
}

void single_quote_end(struct lexer *lexer)
{
    lexer->single_quoted = false;

    // '' Is indeed a word with an empty string.
    lexer_add_char_to_current_token(lexer, '\0');
}

void double_quote_start(struct lexer *lexer)
{
    assert(!lexer->double_quoted);
    lexer->double_quoted = true;
    quote_word(lexer);
}

void double_quote_end(struct lexer *lexer)
{
    lexer->double_quoted = false;

    // "" Is indeed a word with an empty string
    // If no expansion happened.
    lexer_add_char_to_current_token(lexer, '\0');
}

void escape_start(struct lexer *lexer)
{
    assert(!lexer->escaped);
    lexer->escaped = true;
    quote_word(lexer);
}

void escape_end(struct lexer *lexer)
{
    lexer->escaped = false;
}

void lexer_merge(struct lexer *original, struct lexer *copy)
{
    char *tmp = original->cursor->input_string;
    original->cursor->input_string = copy->cursor->input_string;
    copy->cursor->input_string = tmp;

    original->cursor->input_length = copy->cursor->input_length;
    lexer_free(copy);
}
