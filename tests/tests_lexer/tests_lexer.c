#include <criterion/criterion.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/lexer/cursor.h"
#include "../../src/lexer/lexer.h"
#include "../../src/lexer/token.h"
#include "../../src/lexer/lexer_utils.h"
/*---------------------------------------------------------------------------*/
                            //TESTS LEXER STEP1
/*---------------------------------------------------------------------------*/
Test(test_lexer, only_words)
{
    struct lexer *lexer = lexer_init("echo Carl est plus beau que Jules");
    enum token_type expected[] = { TOKEN_WORD, TOKEN_WORD, TOKEN_WORD,
                                   TOKEN_WORD, TOKEN_WORD, TOKEN_WORD,
                                   TOKEN_WORD, TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}

Test(test_lexer, one_word)
{
    struct lexer *lexer = lexer_init("One");
    enum token_type expected[] = { TOKEN_WORD, TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}

Test(test_lexer, no_word)
{
    struct lexer *lexer = lexer_init("");
    enum token_type expected[] = { TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        // printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}

Test(test_lexer, if_statement)
{
    struct lexer *lexer = lexer_init("if true then false fi");
    enum token_type expected[] = { RESERVED_IF, TOKEN_WORD,  RESERVED_THEN,
                                   TOKEN_WORD,  RESERVED_FI, TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        // printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}

Test(test_lexer, if_statement_not_reserved)
{
    struct lexer *lexer = lexer_init("if true then false fi");
    enum token_type expected[] = { TOKEN_WORD, TOKEN_WORD, TOKEN_WORD,
                                   TOKEN_WORD, TOKEN_WORD, TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, false);
        // printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}

Test(test_lexer, if_statement_with_else)
{
    struct lexer *lexer = lexer_init("if true then false else true fi");
    enum token_type expected[] = { RESERVED_IF, TOKEN_WORD,    RESERVED_THEN,
                                   TOKEN_WORD,  RESERVED_ELSE, TOKEN_WORD,
                                   RESERVED_FI, TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        // printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}

Test(test_lexer, if_statement_with_elif_and_else)
{
    struct lexer *lexer =
        lexer_init("if true then false elif notTrue then false else true fi");
    enum token_type expected[] = { RESERVED_IF,   TOKEN_WORD,    RESERVED_THEN,
                                   TOKEN_WORD,    RESERVED_ELIF, TOKEN_WORD,
                                   RESERVED_THEN, TOKEN_WORD,    RESERVED_ELSE,
                                   TOKEN_WORD,    RESERVED_FI,   TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        // printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}

Test(test_lexer, only_ifs)
{
    struct lexer *lexer = lexer_init("if if if if if if");
    enum token_type expected[] = { RESERVED_IF, RESERVED_IF, RESERVED_IF,
                                   RESERVED_IF, RESERVED_IF, RESERVED_IF,
                                   TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        // printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}

Test(test_lexer, semi_colon)
{
    struct lexer *lexer = lexer_init("echo carl; ls -l");
    enum token_type expected[] = { TOKEN_WORD, TOKEN_WORD, OP_SEMI,
                                   TOKEN_WORD, TOKEN_WORD, TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        // printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}

Test(test_lexer, only_semi_colon)
{
    struct lexer *lexer = lexer_init(";;;;");
    enum token_type expected[] = { OP_SEMI, OP_SEMI, OP_SEMI, OP_SEMI,
                                   TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        //        printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}

Test(test_lexer, new_line)
{
    struct lexer *lexer = lexer_init("\n\n\n");
    enum token_type expected[] = { TOKEN_NEWLINE, TOKEN_NEWLINE, TOKEN_NEWLINE,
                                   TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        //        printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}


/*---------------------------------------------------------------------------*/
                            //TESTS LEXER STEP2
/*---------------------------------------------------------------------------*/


        /*--------------------REDIRECTIONS--------------------*/

Test(test_lexer,redirection_great)
{
    struct lexer *lexer = lexer_init("echo test1 test2 > file.txt");
    enum token_type expected[] = {TOKEN_WORD, TOKEN_WORD, TOKEN_WORD, OP_GREAT,TOKEN_WORD
                                   ,TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        //printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}

Test(test_lexer,redirection_great_without_space)
{
    struct lexer *lexer = lexer_init("echo test1 test2>file.txt");
    enum token_type expected[] = {TOKEN_WORD, TOKEN_WORD, TOKEN_WORD, OP_GREAT,TOKEN_WORD
                                   ,TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        //printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}

Test(test_lexer,redirection_great_io_number)
{
    struct lexer *lexer = lexer_init("echo test1 test2 2> file.txt");
    enum token_type expected[] = {TOKEN_WORD, TOKEN_WORD, TOKEN_WORD,IO_NUMBER, OP_GREAT,TOKEN_WORD
                                   ,TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        //printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}


Test(test_lexer,redirection_less_io_number)
{
    struct lexer *lexer = lexer_init("echo test1 test2 2< file.txt");
    enum token_type expected[] = {TOKEN_WORD, TOKEN_WORD, TOKEN_WORD,IO_NUMBER, OP_LESS,TOKEN_WORD
                                   ,TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        //printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}

Test(test_lexer,redirection_dgreat_io_number)
{
    struct lexer *lexer = lexer_init("echo test1 test2 1>> file.txt");
    enum token_type expected[] = {TOKEN_WORD, TOKEN_WORD, TOKEN_WORD,IO_NUMBER, OP_DGREAT,TOKEN_WORD
                                   ,TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        //printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}


Test(test_lexer,redirection_great_and)
{
    struct lexer *lexer = lexer_init("echo test1 test2 2 >& file.txt");
    enum token_type expected[] = {TOKEN_WORD, TOKEN_WORD, TOKEN_WORD,TOKEN_WORD, OP_GREATAND,TOKEN_WORD
                                   ,TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        //printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}


Test(test_lexer,redirection_op_following)
{
    struct lexer *lexer = lexer_init("echo test1 2 >&>> file.txt");
    enum token_type expected[] = {TOKEN_WORD, TOKEN_WORD, TOKEN_WORD, OP_GREATAND,OP_DGREAT,TOKEN_WORD
                                   ,TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        //printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}

    /*--------------------PIPELINE--------------------*/
Test(test_lexer,single_pipe)
{

    struct lexer *lexer = lexer_init("echo test1 | file.txt");
    enum token_type expected[] = {TOKEN_WORD, TOKEN_WORD,OP_PIPE,TOKEN_WORD,TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        //printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}

Test(test_lexer,multiple_pipe)
{

    struct lexer *lexer = lexer_init("echo test1 | cat file | head file2");
    enum token_type expected[] = {TOKEN_WORD, TOKEN_WORD,OP_PIPE,TOKEN_WORD,TOKEN_WORD, OP_PIPE,TOKEN_WORD,TOKEN_WORD,TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        //printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}
    /*--------------------VARIABLES ASSIGNMENT--------------------*/

/*
Test(test_lexer,simple_assignment)
{
    struct lexer *lexer = lexer_init("a=b");
    enum token_type expected[] = {ASSIGNMENT_WORD,TOKEN_WORD,
                                   ,TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        //printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);

}

Test(test_lexer,left_space)
{
    struct lexer *lexer = lexer_init("a =b");
    enum token_type expected[] = {TOKEN_WORD,TOKEN_WORD,
                                   ,TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        //printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}

Test(test_lexer,right_space)
{
    struct lexer *lexer = lexer_init("a= b");
    enum token_type expected[] = {TOKEN_WORD,TOKEN_WORD,
                                   ,TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        //printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}


Test(test_lexer,simple_assignment_with_quotes)
{
    struct lexer *lexer = lexer_init("a='b'");
    enum token_type expected[] = {ASSIGNMENT_WORD,TOKEN_WORD,
                                   ,TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        //printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}

Test(test_lexer,quotes_on_assignmnent_word)
{
    struct lexer *lexer = lexer_init("'a'=b");
    enum token_type expected[] = {TOKEN_WORD,TOKEN_WORD,
                                   ,TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        //printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}


Test(test_lexer,numbers)
{
    struct lexer *lexer = lexer_init("a=12+15");
    enum token_type expected[] = {ASSIGNMENT_WORD,TOKEN_WORD,
                                   ,TOKEN_EOF };
    struct token *token;

    bool eof = false;
    int i = 0;
    while (!eof)
    {
        token = next_token(lexer, true);
        //printf("actual:%d\nexpected: %d\n",token->type,expected[i]);
        cr_assert_eq(token->type, expected[i]);

        eof = token_is_eof(token);
        i++;
        token_free(token);
    }
    lexer_free(lexer);
}
*/
