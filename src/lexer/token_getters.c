#include <stddef.h>

#include "token.h"

bool token_is_reserved_if(struct token *token)
{
    return token->type == RESERVED_IF;
}

bool token_is_reserved_then(struct token *token)
{
    return token->type == RESERVED_THEN;
}

bool token_is_reserved_else(struct token *token)
{
    return token->type == RESERVED_ELSE;
}

bool token_is_reserved_fi(struct token *token)
{
    return token->type == RESERVED_FI;
}

bool token_is_reserved_elif(struct token *token)
{
    return token->type == RESERVED_ELIF;
}

bool token_is_newline(struct token *token)
{
    return token->type == TOKEN_NEWLINE;
}

bool token_is_less(struct token *token)
{
    return token->type == OP_LESS;
}

bool token_is_great(struct token *token)
{
    return token->type == OP_GREAT;
}

bool token_is_dgreat(struct token *token)
{
    return token->type == OP_DGREAT;
}
