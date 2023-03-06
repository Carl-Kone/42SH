#include "token.h"

bool token_is_case(struct token *token)
{
    return token->type == RESERVED_CASE;
}

bool token_is_esac(struct token *token)
{
    return token->type == RESERVED_ESAC;
}
