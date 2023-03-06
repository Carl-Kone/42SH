#ifndef BUILTINS_H
#define BUILTINS_H

#include <stdio.h>

#include "../interpreter/interpreter.h"
#include "../interpreter/variable_list.h"

int builtin_true(char *command_name, char **argv);
int builtin_false(char *command_name, char **argv);
int builtin_echo(char *command_name, char **argv);
int builtin_exit(char *command_name, char **argv,
                 struct interpreter *interpreter);
int builtin_dot(char *command_name, char **argv,
                struct interpreter *interpreter);
int builtin_break(char *command_name, char **argv, int *nb_breaks);
int builtin_continue(char *command_name, char **argv, int *nb_continues);
int builtin_export(char *command_name, char **argv,
                   struct interpreter *interpreter);
int builtin_unset(char *command_name, char **argv,
                  struct interpreter *interpreter);

bool builtin_cd(char *command_name, char **argv,
                struct interpreter *interpreter);

int compute_argc(char **argv);
#endif /* BUILTINS_H */
