#ifndef REDIRECTIONS_H
#define REDIRECTIONS_H

#include "../builtins/builtins.h"
#include "interpreter.h"

void revert_stream(struct environment *environment);

void redirect(struct AST_redirection *redirection,
              struct interpreter *interpreter);

#endif /* !REDIRECTIONS_H */
