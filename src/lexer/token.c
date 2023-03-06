#include "token.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "string.h"

struct token *token_eof(void)
{
    struct token *token = malloc(sizeof(struct token));
    token->type = TOKEN_EOF;
    return token;
}

struct token *token_operator(char *op)
{
    struct token *token = malloc(sizeof(struct token));

    if (strcmp(op, ";") == 0)
        token->type = OP_SEMI;
    else if (strcmp(op, "\n") == 0)
        token->type = TOKEN_NEWLINE;
    else if (strcmp(op, ">") == 0)
        token->type = OP_GREAT;
    else if (strcmp(op, ">>") == 0)
        token->type = OP_DGREAT;
    else if (strcmp(op, "<") == 0)
        token->type = OP_LESS;
    else if (strcmp(op, ">&") == 0)
        token->type = OP_GREATAND;
    else if (strcmp(op, "<&") == 0)
        token->type = OP_LESSAND;
    else if (strcmp(op, ">|") == 0)
        token->type = OP_CLOBBER;
    else if (strcmp(op, "<>") == 0)
        token->type = OP_LESSGREAT;
    else if (strcmp(op, "|") == 0)
        token->type = OP_PIPE;
    else if (strcmp(op, "&&") == 0)
        token->type = OP_ANDAND;
    else if (strcmp(op, "||") == 0)
        token->type = OP_OROR;
    else if (strcmp(op, "(") == 0)
        token->type = OP_LEFT_PAREN;
    else if (strcmp(op, ")") == 0)
        token->type = OP_RIGHT_PAREN;
    else if (strcmp(op, ";;") == 0)
        token->type = OP_SEMI_SEMI;
    else
    {
        printf("token_operator ERROR\n");
        exit(2);
    }

    return token;
}

struct token *token_word(char *word)
{
    struct token *token = malloc(sizeof(struct token));
    token->type = TOKEN_WORD;

    token->value.word = strdup(word);
    return token;
}

struct token *token_io_number(char *io_number)
{
    struct token *token = malloc(sizeof(struct token));
    token->type = IO_NUMBER;
    token->value.io_number = atoi(io_number);
    return token;
}

void token_free(struct token *token)
{
    if (token->type == TOKEN_WORD || token->type == TOKEN_ASSIGNMENT_WORD)
        free(token->value.word);

    free(token);
}

static struct token *token_to_reserved_bis(struct token *new_token, char *value,
                                           struct token *token)
{
    if (strcmp(value, "done") == 0)
        new_token->type = RESERVED_DONE;
    else if (strcmp(value, "in") == 0)
        new_token->type = RESERVED_IN;
    else if (strcmp(value, "{") == 0)
        new_token->type = RESERVED_LEFT_CURLY;
    else if (strcmp(value, "}") == 0)
        new_token->type = RESERVED_RIGHT_CURLY;
    else if (strcmp(value, "case") == 0)
        new_token->type = RESERVED_CASE;
    else if (strcmp(value, "esac") == 0)
        new_token->type = RESERVED_ESAC;
    else
    {
        new_token->type = TOKEN_WORD;
        new_token->value.word = strdup(token->value.word);
    }

    return new_token;
}

struct token *token_to_reserved(struct token *token)
{
    if (token->type != TOKEN_WORD)
    {
        printf("token_to_reserved ERROR\n");
        exit(-1);
    }

    struct token *new_token = malloc(sizeof(struct token));
    char *value = token->value.word;

    if (strcmp(value, "if") == 0)
        new_token->type = RESERVED_IF;
    else if (strcmp(value, "then") == 0)
        new_token->type = RESERVED_THEN;
    else if (strcmp(value, "elif") == 0)
        new_token->type = RESERVED_ELIF;
    else if (strcmp(value, "else") == 0)
        new_token->type = RESERVED_ELSE;
    else if (strcmp(value, "fi") == 0)
        new_token->type = RESERVED_FI;
    else if (strcmp(value, "while") == 0)
        new_token->type = RESERVED_WHILE;
    else if (strcmp(value, "until") == 0)
        new_token->type = RESERVED_UNTIL;
    else if (strcmp(value, "for") == 0)
        new_token->type = RESERVED_FOR;
    else if (strcmp(value, "!") == 0)
        new_token->type = RESERVED_NEGATE;
    else if (strcmp(value, "do") == 0)
        new_token->type = RESERVED_DO;
    else
        token_to_reserved_bis(new_token, value, token);

    return new_token;
}

struct token *token_to_assignment(struct token *token,
                                  bool has_quoted_or_escaped,
                                  size_t first_quote_or_escape_idx)
{
    if (token->type != TOKEN_WORD)
    {
        printf("token_to_assignment ERROR\n");
        exit(-1);
    }

    struct token *new_token = malloc(sizeof(struct token));
    char *value = token->value.word;
    char *equal_char = strchr(value, '=');

    if (equal_char && !(value[0] >= '0' && value[0] <= '9')
        && (!has_quoted_or_escaped
            || (size_t)(equal_char - value) < first_quote_or_escape_idx))
    {
        new_token->type = TOKEN_ASSIGNMENT_WORD;
        new_token->value.word = strdup(token->value.word);
    }
    else
    {
        new_token->type = TOKEN_WORD;
        new_token->value.word = strdup(token->value.word);
    }

    return new_token;
}

bool token_is_word(struct token *token)
{
    return token->type == TOKEN_WORD;
}

bool token_is_semi_colon(struct token *token)
{
    return token->type == OP_SEMI;
}

bool token_is_eof(struct token *token)
{
    return token->type == TOKEN_EOF;
}
