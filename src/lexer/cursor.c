#include "cursor.h"

#include <stdlib.h>
#include <string.h>

void cursor_extend(struct cursor *cursor, char *input_string)
{
    size_t new_len = strlen(input_string);
    size_t current_len = cursor->input_length;
    cursor->input_string =
        realloc(cursor->input_string, current_len + new_len + 1);
    strcat(cursor->input_string, input_string);
    cursor->input_length += new_len;
}

struct cursor *cursor_init(char *input_string)
{
    struct cursor *cursor = malloc(sizeof(struct cursor));
    if (!cursor)
        return NULL;
    cursor->input_length = strlen(input_string);
    cursor->current_index = 0;
    cursor->input_string = strdup(input_string);
    cursor->previous_char = '\0';

    return cursor;
}

bool cursor_is_eof(struct cursor *cursor)
{
    return cursor->current_index >= cursor->input_length;
}

char cursor_current_char(struct cursor *cursor)
{
    if (cursor->current_index >= cursor->input_length)
        return '\0';
    return cursor->input_string[cursor->current_index];
}

void cursor_advance_char(struct cursor *cursor)
{
    cursor->previous_char = cursor_current_char(cursor);
    cursor->current_index++;
}

void cursor_free(struct cursor *cursor)
{
    free(cursor->input_string);
    free(cursor);
}

char cursor_previous_char(struct cursor *cursor)
{
    return cursor->previous_char;
}
