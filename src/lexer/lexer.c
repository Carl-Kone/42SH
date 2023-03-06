#include "lexer.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../interpreter/environment.h"
#include "../interpreter/variable_list.h"
#include "cursor.h"
#include "lexer_utils.h"
#include "token.h"

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
/// 1)  If the end of input is recognized,
///     the current token shall be delimited.
///     If there is no current token, the end-of-input indicator shall be
///     returned as the token.
static bool rule_1(struct lexer *lexer)
{
    if (!cursor_is_eof(lexer->cursor))
    {
        return false;
    }

    if ((lexer->single_quoted || lexer->double_quoted || lexer->escaped)
        && lexer->is_prompt && !lexer->input_end_reached)
    {
        char *new_str = prompt_for_input();
        if (!new_str)
            lexer->input_end_reached = true;
        else
        {
            // cursor_free(lexer->cursor);
            // lexer->cursor = cursor_init(new_str);
            // free(new_str);
            cursor_extend(lexer->cursor, new_str);
            free(new_str);
            return true;
        }
    }
    if (lexer->single_quoted || lexer->double_quoted)
    {
        fprintf(stderr, "Unexpected EOF\n");
        exit(2);
    }

    if (lexer->escaped)
    {
        lexer_add_char_to_current_token(lexer, '\\');
        lexer->previous_part_of_word = true;
    }

    if (lexer->previous_part_of_word)
        lexer_add_char_to_current_token(lexer, '\0');

    struct token *token =
        lexer->current_token ? delimit_current_token(lexer) : token_eof();

    add_token(lexer, token);
    advance_char(lexer);
    return true;
}

/// 2)  If the previous character was used as part of an operator and
///     the current character is not quoted and can be used with the current
///     characters to form an operator,
///     it shall be used as part of that (operator) token.
static bool rule_2(struct lexer *lexer)
{
    char current_char = cursor_current_char(lexer->cursor);
    if (!lexer->previous_part_of_op || lexer->single_quoted
        || lexer->double_quoted || lexer->escaped
        || !can_be_used_to_form_op(lexer->current_token, current_char))
        return false;

    lexer_add_char_to_current_token(lexer, current_char);
    advance_char(lexer);
    return false;
}

/// 3)  If the previous character was used as part of an operator and
///     the current character cannot be used with the current characters
///     to form an operator, the operator containing the previous character
///     shall be delimited.
static bool rule_3(struct lexer *lexer)
{
    char current_char = cursor_current_char(lexer->cursor);
    if (!lexer->previous_part_of_op
        || can_be_used_to_form_op(lexer->current_token, current_char))
        return false;

    struct token *token = delimit_current_token(lexer);
    add_token(lexer, token);
    return true;
}

static bool rule_4_backslash(struct lexer *lexer)
{
    char current_char = cursor_current_char(lexer->cursor);
    if (current_char != '\\' || lexer->single_quoted || lexer->escaped)
        return false;

    if (!lexer->is_in_expansion_mode)
        lexer_add_char_to_current_token(lexer, current_char);

    advance_char(lexer);
    current_char = cursor_current_char(lexer->cursor);
    bool is_word = lexer->previous_part_of_word;
    escape_start(lexer);

    if (lexer->is_in_expansion_mode)
    {
        if (current_char == '\n')
        {
            advance_char(lexer);
            lexer->previous_part_of_word = is_word;
        }
        else if (lexer->double_quoted
                 && !(current_char == '\\' || current_char == '"'
                      || current_char == '`'))
        {
            escape_end(lexer);
            lexer_add_char_to_current_token(lexer, '\\');
        }
    }
    else
    {
        if (current_char == '\n')
        {
            advance_char(lexer);
            if (cursor_is_eof(lexer->cursor) && lexer->is_prompt
                && !lexer->input_end_reached)
            {
                char *new_str = prompt_for_input();
                if (!new_str)
                    lexer->input_end_reached = true;
                else
                {
                    cursor_extend(lexer->cursor, new_str);
                    free(new_str);
                }
            }

            lexer_add_char_to_current_token(lexer, '\n');
        }
    }

    return true;
}

static bool rule_4_double_quotes(struct lexer *lexer)
{
    char current_char = cursor_current_char(lexer->cursor);
    if (current_char != '"' || lexer->single_quoted || lexer->escaped)
        return false;

    if (!lexer->double_quoted)
    {
        double_quote_start(lexer);
        lexer->previous_part_of_word = true;
    }
    else
        double_quote_end(lexer);

    if (!lexer->is_in_expansion_mode)
        lexer_add_char_to_current_token(lexer, current_char);

    advance_char(lexer);
    return true;
}

static bool rule_4_single_quote(struct lexer *lexer)
{
    char current_char = cursor_current_char(lexer->cursor);
    if (current_char != '\'' || lexer->double_quoted || lexer->escaped)
        return false;

    if (!lexer->single_quoted)
    {
        single_quote_start(lexer);
        lexer->previous_part_of_word = true;
    }
    else
        single_quote_end(lexer);

    if (!lexer->is_in_expansion_mode)
        lexer_add_char_to_current_token(lexer, current_char);

    advance_char(lexer);
    return true;
}

/// 4)  If the current character is backslash, single-quote, or double-quote
///     ( '\', '", or ' )' and it is not quoted, it shall affect quoting for
///     subsequent characters up to the end of the quoted text. The rules for
///     quoting are as described in Quoting. During token recognition no
///     substitutions shall be actually performed, and the result token shall
///     contain exactly the characters that appear in the input
///     (except for <newline> joining), unmodified, including any embedded or
///     enclosing quotes or substitution operators, between the quote mark and
///     the end of the quoted text. The token shall not be delimited by the
///     end of the quoted field.
static bool rule_4(struct lexer *lexer)
{
    if (!rule_4_backslash(lexer) && !rule_4_single_quote(lexer)
        && !rule_4_double_quotes(lexer))
        return false;

    return true;
}

static bool char_is_valid_for_variable(char c)
{
    bool is_alpha = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    bool is_numeric = c >= '0' && c <= '9';
    bool is_other_valid = c == '_';

    return is_alpha || is_numeric || is_other_valid;
}

static void get_name_number(struct lexer *lexer, char *name, bool brackets)
{
    char current_char = cursor_current_char(lexer->cursor);
    size_t i = 0;

    name[i++] = current_char;
    name[i] = '\0';
    cursor_advance_char(lexer->cursor);

    if (brackets)
    {
        while (!cursor_is_eof(lexer->cursor) && current_char >= '0'
               && current_char <= '9')
        {
            name[i++] = current_char;
            name[i] = '\0';
            cursor_advance_char(lexer->cursor);
            current_char = cursor_current_char(lexer->cursor);
        }
    }
}

static void get_name_special(struct lexer *lexer, char *name, bool brackets)
{
    assert(brackets == brackets);
    char current_char = cursor_current_char(lexer->cursor);
    size_t i = 0;
    name[i++] = current_char;
    name[i] = '\0';
    cursor_advance_char(lexer->cursor);
}

static void get_name_other(struct lexer *lexer, char *name, bool brackets)
{
    assert(brackets == brackets);
    char current_char = cursor_current_char(lexer->cursor);
    size_t i = 0;
    while (!cursor_is_eof(lexer->cursor)
           && char_is_valid_for_variable(current_char))
    {
        name[i++] = current_char;
        name[i] = '\0';
        cursor_advance_char(lexer->cursor);
        current_char = cursor_current_char(lexer->cursor);
    }
}

static void brackets_check(struct lexer *lexer, bool brackets)
{
    char current_char = cursor_current_char(lexer->cursor);
    if (brackets)
    {
        current_char = cursor_current_char(lexer->cursor);
        if (!cursor_is_eof(lexer->cursor) && current_char == '}')
            cursor_advance_char(lexer->cursor);
        else
        {
            fprintf(stderr, "bad substitution %c\n", current_char);
            exit(2);
        }
    }
}

static char *rule_5_get_name(struct lexer *lexer)
{
    cursor_advance_char(lexer->cursor);
    char current_char = cursor_current_char(lexer->cursor);
    if (cursor_is_eof(lexer->cursor))
        return NULL;

    char *name = malloc(1000);
    name[0] = '\0';

    bool brackets = false;
    if (current_char == '{')
    {
        brackets = true;
        cursor_advance_char(lexer->cursor);
        current_char = cursor_current_char(lexer->cursor);
    }

    // $0..n
    if (current_char >= '0' && current_char <= '9')
        get_name_number(lexer, name, brackets);

    // $@
    else if (current_char == '@' || current_char == '*' || current_char == '?'
             || current_char == '#')
        get_name_special(lexer, name, brackets);

    // User defined variable. + $RANDOM...
    else
        get_name_other(lexer, name, brackets);

    brackets_check(lexer, brackets);

    // No var name after $, juste add $ to current name.
    if (strcmp(name, "") == 0)
    {
        lexer_add_char_to_current_token(lexer, '$');
        free(name);
        return NULL;
    }

    return name;
}

/// 5)  If the current character is an unquoted '$' or '`', the shell shall
///     identify the start of any candidates for parameter expansion
///     (Parameter Expansion), command substitution (Command Substitution),
///     or arithmetic expansion (Arithmetic Expansion) from their introductory
///     unquoted character sequences: '$' or "${", "$(" or '`', and "$((",
///     respectively. The shell shall read sufficient input to determine the
///     end of the unit to be expanded (as explained in the cited sections).
///     While processing the characters, if instances of expansions or quoting
///     are found nested within the substitution, the shell shall recursively
///     process them in the manner specified for the construct that is found.
///     The characters found from the beginning of the substitution to its end,
///     allowing for any recursion necessary to recognize embedded constructs,
///     shall be included unmodified in the result token,
///     including any embedded or enclosing substitution operators or quotes.
///     The token shall not be delimited by the end of the substitution
static bool rule_5(struct lexer *lexer)
{
    char current_char = cursor_current_char(lexer->cursor);
    if (current_char != '$' || lexer->single_quoted || lexer->escaped
        || !lexer->is_in_expansion_mode)
        return false;

    char *name = rule_5_get_name(lexer);
    if (!name)
        return true;

    char *value = variable_list_get(lexer->environment->variable_list, name);
    char *value_expanded = environment_expansion(lexer->environment, value);

    if (value_expanded)
    {
        for (size_t j = 0; value_expanded[j]; j++)
            lexer_add_char_to_current_token(lexer, value_expanded[j]);
        lexer->previous_part_of_word = true;
    }

    free(value);
    free(value_expanded);
    free(name);
    return true;
}

/// 6)  If the current character is not quoted and can be used as the first
///     character of a new operator, the current token (if any) shall be
///     delimited. The current character shall be used as the beginning of the
///     next (operator) token.
static bool rule_6(struct lexer *lexer)
{
    char current_char = cursor_current_char(lexer->cursor);

    if (lexer->single_quoted || lexer->double_quoted || lexer->escaped
        || !can_be_used_as_op_first_char(current_char))
        return false;

    if (lexer->current_token)
    {
        struct token *token = delimit_current_token(lexer);
        add_token(lexer, token);
    }

    lexer_add_char_to_current_token(lexer, current_char);
    lexer->previous_part_of_op = true;
    advance_char(lexer);

    return true;
}

/// 7)  If the current character is an unquoted <blank>, any token containing
///     the previous character is delimited and the current character
///     shall be discarded.
static bool rule_7(struct lexer *lexer)
{
    char current_char = cursor_current_char(lexer->cursor);

    if (lexer->single_quoted || lexer->double_quoted || lexer->escaped
        || !is_blank_char(current_char))
        return false;

    if (lexer->current_token)
    {
        struct token *token = delimit_current_token(lexer);
        add_token(lexer, token);
    }

    advance_char(lexer);
    return true;
}

/// 8)  If the previous character was part of a word, the current character
///     shall be appended to that word.
static bool rule_8(struct lexer *lexer)
{
    if (!lexer->previous_part_of_word)
        return false;

    char current_char = cursor_current_char(lexer->cursor);
    lexer_add_char_to_current_token(lexer, current_char);

    advance_char(lexer);

    return true;
}

/// 9)  If the current character is a '#', it and all subsequent characters
///     up to, but excluding, the next <newline> shall be discarded as a
///     comment. The <newline> that ends the line is not considered part of
///     the comment.
static bool rule_9(struct lexer *lexer)
{
    char current_char = cursor_current_char(lexer->cursor);
    if (current_char != '#' || lexer->escaped || lexer->single_quoted
        || lexer->double_quoted)
        return false;

    advance_char(lexer);
    while (cursor_current_char(lexer->cursor) != '\n'
           && !cursor_is_eof(lexer->cursor))
    {
        advance_char(lexer);
    }

    return true;
}

/// 10) The current character is used as the start of a new word.
static bool rule_10(struct lexer *lexer)
{
    char current_char = cursor_current_char(lexer->cursor);
    lexer_add_char_to_current_token(lexer, current_char);
    lexer->previous_part_of_word = true;
    advance_char(lexer);
    return true;
}

static void lexer_handle_char(struct lexer *lexer)
{
    if (rule_1(lexer))
        return;
    if (rule_2(lexer))
        return;
    if (rule_3(lexer))
        return;
    if (rule_4(lexer))
        return;
    if (rule_5(lexer))
        return;
    if (rule_6(lexer))
        return;
    if (rule_7(lexer))
        return;
    if (rule_8(lexer))
        return;
    if (rule_9(lexer))
        return;
    if (rule_10(lexer))
        return;
}

static void lexer_advance_char(struct lexer *lexer)
{
    lexer_handle_char(lexer);
    // cursor_advance_char(lexer->cursor);
}

/// Convert a `TOKEN_WORD` token to the desired type if possible.
/// Conversion takes occurs in place: the lexer->token will be modified.
void token_conversion(struct lexer *lexer, enum filter_token expected_token)
{
    if (!token_is_word(lexer->token) || expected_token == FILTER_WORD)
        return;

    if (expected_token & FILTER_RESERVED_WORD && !lexer->word_is_quoted)
    {
        struct token *previous_token = lexer->token;
        lexer->token = token_to_reserved(previous_token);
        token_free(previous_token);

        if (previous_token == lexer->token)
            return;
    }
    if (token_is_word(lexer->token) && expected_token & FILTER_ASSIGNMENT_WORD)
    {
        struct token *previous_token = lexer->token;
        lexer->token = token_to_assignment(lexer->token, lexer->word_is_quoted,
                                           lexer->first_quote_index);
        token_free(previous_token);

        if (previous_token == lexer->token)
            return;
    }
    else
    {
        return;
    }
}

struct token *next_token(struct lexer *lexer, enum filter_token expected_token)
{
    // Progress in lexing until a
    // token is built.
    while (!lexer->token)
        lexer_advance_char(lexer);

    token_conversion(lexer, expected_token);

    struct token *token = lexer->token;
    lexer->token = NULL;
    lexer->word_is_quoted = false;
    lexer->first_quote_index = 0;
    // free(lexer->current_token);
    // lexer->current_token = NULL;
    return token;
}

struct lexer *lexer_init(char *input)
{
    struct lexer *lexer = malloc(sizeof(struct lexer));
    lexer->cursor = cursor_init(input);
    lexer->io_here_recognized = false;
    lexer->current_token = NULL;
    lexer->token = NULL;
    lexer->previous_part_of_op = false;
    lexer->single_quoted = false;
    lexer->double_quoted = false;
    lexer->previous_part_of_word = false;
    lexer->word_is_quoted = false;
    lexer->escaped = false;
    lexer->first_quote_index = 0;
    lexer->is_in_expansion_mode = false;
    lexer->environment = NULL;
    lexer->reading_var_name = false;
    lexer->input_end_reached = false;
    lexer->is_prompt = false;
    lexer->current_var_name = NULL;
    return lexer;
}

void lexer_free(struct lexer *lexer)
{
    cursor_free(lexer->cursor);
    free(lexer->current_token);
    free(lexer->token);
    free(lexer);
}
