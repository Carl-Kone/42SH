cmake_minimum_required(VERSION 3.22)
project(42sh LANGUAGES C)

add_compile_options(-Wall -Wextra -Werror -Wvla -pedantic -std=c99 -D_POSIX_C_SOURCE=200809L)
add_compile_options(-g)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

# BUILTIN
set(builtin_src src/builtins/true_false.c src/builtins/echo.c
    src/builtins/exit.c src/builtins/dot.c src/builtins/break.c
    src/builtins/continue.c src/builtins/export.c src/builtins/cd.c
    src/builtins/unset.c)

# LEXER
set(lexer_src src/lexer/lexer.c src/lexer/token.c src/lexer/cursor.c
src/lexer/token_getters.c src/lexer/lexer_utils.c src/lexer/lexer_utils_2.c src/lexer/token_2.c
src/lexer/token_3.c src/lexer/token_4.c src/lexer/token_5.c)
set(lexer_test_src tests/tests_lexer/tests_lexer.c)

# PARSER
set(parser_src src/parser/ast_frees.c src/parser/ast_prints.c
    src/parser/ast_inits.c src/parser/parser.c src/parser/ast_add_funcs.c
    src/parser/ast_add_funcs_2.c 
    src/parser/utils.c src/parser/ast_copy.c)

# INTERPRETER
set(interpreter_src src/interpreter/interpreter.c src/interpreter/redirections.c src/interpreter/environment.c 
    src/interpreter/variable_list.c src/interpreter/function_list.c)

# MAIN
set(42sh_src src/main/main.c src/main/utils.c)
add_executable(42sh ${42sh_src} ${lexer_src} ${parser_src} ${interpreter_src} ${builtin_src})
