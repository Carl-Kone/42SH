#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdbool.h>

#include "../parser/ast.h"
#include "environment.h"

struct interpreter
{
    struct environment *environment;
    int nb_breaks;
    int nb_loops;
    int nb_continues;
};

int interprete_ast(struct AST *ast, struct interpreter *interpreter);

struct interpreter *interpreter_init(struct environment *env);

void dup_streams(struct environment *env);

void interpreter_free(struct interpreter *interpreter);

#endif /* !INTERPRETER_H */
