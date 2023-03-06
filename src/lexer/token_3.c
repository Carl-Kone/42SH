#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#include "token.h"

bool token_is_less_and(struct token *token)
{
    return token->type == OP_LESSAND;
}

bool token_is_great_and(struct token *token)
{
    return token->type == OP_GREATAND;
}

bool token_is_less_great(struct token *token)
{
    return token->type == OP_LESSGREAT;
}

bool token_is_clobber(struct token *token)
{
    return token->type == OP_CLOBBER;
}

char *token_get_word_value(struct token *token)
{
    if (token->type != TOKEN_WORD)
        return NULL;

    return token->value.word;
}

static void token_print_4(struct token *token)
{
    switch (token->type)
    {
    case RESERVED_LEFT_CURLY:
        printf("RESERVED_LEFT_CURLY\n");
        break;
    case RESERVED_RIGHT_CURLY:
        printf("RESERVED_RIGT_CURLY\n");
        break;
    case OP_SEMI_SEMI:
        printf("OP_SEMI_SEMI\n");
        break;
    case RESERVED_CASE:
        printf("RESERVED_CASE\n");
        break;
    case RESERVED_ESAC:
        printf("RESERVED_ESAC\n");
        break;
    default:
        fprintf(stderr, "error in token_print\n");
    }
}

static void token_print_3(struct token *token)
{
    switch (token->type)
    {
    case RESERVED_WHILE:
        printf("RESERVED_WHILE\n");
        break;
    case RESERVED_UNTIL:
        printf("RESERVED_UNTIL\n");
        break;
    case OP_ANDAND:
        printf("OP_ANDAND\n");
        break;
    case OP_OROR:
        printf("OP_OROR\n");
        break;
    case RESERVED_FOR:
        printf("RESERVED_FOR\n");
        break;
    case RESERVED_DO:
        printf("RESERVED_DO\n");
        break;
    case RESERVED_DONE:
        printf("RESERVED_DONE\n");
        break;
    case RESERVED_IN:
        printf("RESERVED_IN\n");
        break;
    case OP_LEFT_PAREN:
        printf("OP_LEFT_PAREN\n");
        break;
    case OP_RIGHT_PAREN:
        printf("OP_RIGHT_PAREN\n");
        break;
    default:
        token_print_4(token);
    }
}

static void token_print_2(struct token *token)
{
    switch (token->type)
    {
    case OP_GREAT:
        printf("OP_GREAT\n");
        break;
    case OP_DGREAT:
        printf("OP_DGREAT\n");
        break;
    case OP_LESSAND:
        printf("OP_LESSAND\n");
        break;
    case OP_GREATAND:
        printf("OP_GREATAND\n");
        break;
    case OP_LESSGREAT:
        printf("OP_LESSGREAT\n");
        break;
    case OP_CLOBBER:
        printf("OP_CLOBBE\n");
        break;
    case IO_NUMBER:
        printf("IO_NUMBER: %d\n", token->value.io_number);
        break;
    case OP_PIPE:
        printf("OP_PIPE\n");
        break;
    case TOKEN_ASSIGNMENT_WORD:
        printf("TOKEN_ASSIGNMENT_WORD: %s\n", token->value.word);
        break;
    case RESERVED_NEGATE:
        printf("RESERVED_NEGATE\n");
        break;
    default:
        token_print_3(token);
    }
}

void token_print(struct token *token)
{
    switch (token->type)
    {
    case TOKEN_WORD:
        printf("WORD %s\n", token->value.word);
        break;
    case TOKEN_EOF:
        printf("TOKEN_EOF\n");
        break;
    case TOKEN_NEWLINE:
        printf("TOKEN_NEWLINE\n");
        break;
    case RESERVED_IF:
        printf("RESERVED_IF\n");
        break;
    case RESERVED_FI:
        printf("RESERVED_FI\n");
        break;
    case RESERVED_ELIF:
        printf("RESERVED_ELIF\n");
        break;
    case RESERVED_ELSE:
        printf("RESERVED_ELSE\n");
        break;
    case RESERVED_THEN:
        printf("RESERVED_THEN\n");
        break;
    case OP_SEMI:
        printf("OP_SEMI\n");
        break;
    case OP_LESS:
        printf("OP_LESS\n");
        break;
    default:
        token_print_2(token);
    }
}
