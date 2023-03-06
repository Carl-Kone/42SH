#include "utils.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "string.h"

enum and_or_type get_and_or_type(struct parser *parser)
{
    if (eat(parser, token_is_oror))
        return OR;
    if (eat(parser, token_is_andand))
        return AND;
    return NOTHING;
}

bool eat_newlines(struct parser *parser)
{
    while (eat(parser, token_is_newline))
        continue;
    return true;
}

// checks if the stream has either a semi-colon, or minimum 1 newline and an
// or (the latter adding the new and_or in compound_list)
bool compound_list_condition(struct parser *parser, struct AST *compound_list)
{
    if (!eat(parser, token_is_semi_colon) && !eat(parser, token_is_newline))
        return false;

    while (eat(parser, token_is_newline))
        continue;

    if (!and_or(parser, compound_list))
        return false;
    return true;
}

bool eat(struct parser *parser, bool (*predicate)(struct token *token))
{
    if (predicate(parser->stream))
    {
        token_free(parser->stream);
        parser->stream = parser_next_token(parser);
    }
    else
        return false;

    return parser->stream;
}

// might put that in an utils or something
// is supposed to throw errors on false
bool is_newline_or_eof(struct parser *parser)
{
    if (parser->stream->type == TOKEN_EOF
        || parser->stream->type == TOKEN_NEWLINE)
    {
        return true;
    }
    return false;
}

static char *prompt_for_input(void)
{
    char *buffer = NULL;
    size_t size;
    if (getline(&buffer, &size, stdin) != -1)
    {
        // printf("Getline: %s\n", buffer);
        return buffer;
    }

    free(buffer);
    return NULL;
}

void look_ahead(struct parser *parser)
{
    if (parser->look_ahead)
    {
        fprintf(stderr, "already a look_ahead\n");
        exit(2);
    }

    enum filter_token filter = parser->preceding_command
        ? parser->filter_token | FILTER_ASSIGNMENT_WORD
        : parser->filter_token;

    parser->look_ahead = next_token(parser->lexer, filter);
}

struct lexer *lexer_copy(struct parser *parser)
{
    struct lexer *cpy = calloc(1, sizeof(struct lexer));
    memcpy(cpy, parser->lexer, sizeof(struct lexer));
    if (parser->lexer->current_token)
        cpy->current_token = strdup(parser->lexer->current_token);
    assert(!cpy->token);
    assert(!cpy->current_var_name);

    struct cursor *cursor_cpy = calloc(1, sizeof(struct cursor));
    memcpy(cursor_cpy, parser->lexer->cursor, sizeof(struct cursor));
    cursor_cpy->input_string = strdup(parser->lexer->cursor->input_string);
    cpy->cursor = cursor_cpy;

    return cpy;
}

struct token *parser_next_token(struct parser *parser)
{
    enum filter_token filter = parser->preceding_command
        ? parser->filter_token | FILTER_ASSIGNMENT_WORD
        : parser->filter_token;

    struct token *token = parser->look_ahead
        ? parser->look_ahead
        : next_token(parser->lexer, filter);

    if (parser->look_ahead)
        parser->look_ahead = NULL;

    if (token_is_eof(token) && parser->is_in_prompt_mode)
    {
        char *new_str = prompt_for_input();
        if (new_str)
        {
            token_free(token);
            lexer_free(parser->lexer);
            parser->lexer = lexer_init(new_str);
            parser->lexer->is_prompt = true;
            free(new_str);
            token = parser_next_token(parser);
        }
        else
        {
            parser->end_of_file_reached = true;
        }
    }
    else if (token_is_eof(token) && !parser->is_in_prompt_mode)
    {
        parser->end_of_file_reached = true;
    }

    return token;
}
