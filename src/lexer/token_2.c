#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "string.h"
#include "token.h"

bool token_is_redirection_operator(struct token *token)
{
    switch (token->type)
    {
    case OP_LESS:
    case OP_GREAT:
    case OP_DGREAT:
    case OP_LESSAND:
    case OP_GREATAND:
    case OP_LESSGREAT:
    case OP_CLOBBER:
        return true;
    default:
        return false;
    }
}

bool token_is_io_number(struct token *token)
{
    return token->type == IO_NUMBER;
}

bool token_is_pipe(struct token *token)
{
    return token->type == OP_PIPE;
}

int token_get_io_number(struct token *token)
{
    assert(token->type == IO_NUMBER);

    return token->value.io_number;
}

bool token_is_assignment_word(struct token *token)
{
    return token->type == TOKEN_ASSIGNMENT_WORD;
}

char *token_get_assignment_word_value(struct token *token)
{
    if (token->type != TOKEN_ASSIGNMENT_WORD)
        return NULL;

    return token->value.word;
}

bool token_is_reserved_negate(struct token *token)
{
    return token->type == RESERVED_NEGATE;
}

bool token_is_reserved_while(struct token *token)
{
    return token->type == RESERVED_WHILE;
}

bool token_is_reserved_until(struct token *token)
{
    return token->type == RESERVED_UNTIL;
}

bool token_is_andand(struct token *token)
{
    return token->type == OP_ANDAND;
}
