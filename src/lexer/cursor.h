#ifndef CURSOR_H
#define CURSOR_H

#include <stdbool.h>
#include <stddef.h>

/// Cursor abstraction used to
/// manipulate some text.
struct cursor
{
    size_t input_length;
    char *input_string;
    size_t current_index;
    char previous_char;
};

bool cursor_is_eof(struct cursor *cursor);
char cursor_current_char(struct cursor *cursor);
void cursor_advance_char(struct cursor *cursor);
struct cursor *cursor_init(char *input_string);
void cursor_free(struct cursor *cursor);
char cursor_previous_char(struct cursor *cursor);
void cursor_extend(struct cursor *cursor, char *input_string);

#endif /* ! CURSOR_H */
