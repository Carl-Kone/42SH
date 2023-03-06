#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "string.h"
#include "token.h"

bool token_is_oror(struct token *token)
{
    return token->type == OP_OROR;
}

bool token_is_reserved_for(struct token *token)
{
    return token->type == RESERVED_FOR;
}

bool token_is_reserved_do(struct token *token)
{
    return token->type == RESERVED_DO;
}

bool token_is_reserved_done(struct token *token)
{
    return token->type == RESERVED_DONE;
}

bool token_is_reserved_in(struct token *token)
{
    return token->type == RESERVED_IN;
}

bool token_is_left_paren(struct token *token)
{
    return token->type == OP_LEFT_PAREN;
}

bool token_is_right_paren(struct token *token)
{
    return token->type == OP_RIGHT_PAREN;
}

bool token_is_left_curly(struct token *token)
{
    return token->type == RESERVED_LEFT_CURLY;
}

bool token_is_right_curly(struct token *token)
{
    return token->type == RESERVED_RIGHT_CURLY;
}

bool token_is_semi_semi(struct token *token)
{
    return token->type == OP_SEMI_SEMI;
}
