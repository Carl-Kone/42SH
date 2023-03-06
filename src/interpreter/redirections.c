#include "redirections.h"

#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "environment.h"
#include "interpreter.h"

void revert_stream(struct environment *environment)
{
    fflush(stdout);
    fflush(stderr);
    dup2(environment->out, fileno(stdout));
    dup2(environment->err, fileno(stderr));
    dup2(environment->in, fileno(stdin));
}

static int fd_str_to_int(char *path)
{
    if (!strcmp(path, "-"))
        return -1;

    char *end_ptr;
    int fd = strtol(path, &end_ptr, 10);
    if (path == end_ptr)
    {
        fprintf(stderr, "42sh: %s: ambiguous redirect\n", path);
        exit(1);
    }
    return fd;
}

static void redirect_duplicating_input(int path, int io)
{
    if (path == -1)
    {
        close(io);
        return;
    }

    if (io == -1)
        io = 0;

    if (fcntl(path, F_GETFL) == -1 || dup2(path, io) == -1)
    {
        fprintf(stderr, "42sh: %d: Bad file descriptor\n", path);
        exit(1);
    }
}

static void redirect_duplicating_output(int path, int io)
{
    if (path == -1)
    {
        close(io);
        return;
    }

    if (io == -1)
        io = 1;

    if (fcntl(path, F_GETFL) == -1 || dup2(path, io) == -1)
    {
        fprintf(stderr, "42sh: %d: Bad file descriptor\n", path);
        exit(1);
    }
}

static void redirect_fd(char *path, int io)
{
    if (io == -1)
        io = 0;

    int new_out = open(path, O_RDWR | O_CREAT, 0666);
    if (new_out == -1)
    {
        fprintf(stderr, "42sh: %s: No such file or directory\n", path);
        return;
    }

    if (dup2(new_out, io) == -1)
    {
        fprintf(stderr, "couldn't redirect stdout!\n");
        return;
    }
    close(new_out);
}

static void redirect_input(char *path, int io)
{
    if (io == -1)
        io = 0;

    int new_out = open(path, O_RDONLY, 0666);
    if (new_out == -1)
    {
        fprintf(stderr, "42sh: %s: No such file or directory\n", path);
        return;
    }

    if (dup2(new_out, io) == -1)
    {
        printf("%d\n", io);
        fprintf(stderr, "couldn't redirect stdout!\n");
        return;
    }
    close(new_out);
}

static void redirect_appending_output(char *path, int io)
{
    if (io == -1)
        io = 1;

    int new_out = open(path, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (new_out == -1)
    {
        fprintf(stderr, "couldn't open/create file!\n");
        return;
    }

    if (dup2(new_out, io) == -1)
    {
        printf("%d\n", io);
        fprintf(stderr, "couldn't redirect stdout!\n");
        return;
    }
    close(new_out);
}

static void redirect_output(char *path, int io)
{
    if (io == -1)
        io = 1;

    int new_out = open(path, O_WRONLY | O_CREAT, 0666);
    if (new_out == -1)
    {
        fprintf(stderr, "couldn't open/create file!\n");
        return;
    }

    if (dup2(new_out, io) == -1)
    {
        printf("%d\n", io);
        fprintf(stderr, "couldn't redirect stdout!\n");
        return;
    }
    close(new_out);
}

// returns FD to give in order to undo redirections
void redirect(struct AST_redirection *redirection,
              struct interpreter *interpreter)
{
    char *word =
        environment_expansion(interpreter->environment, redirection->word);
    switch (redirection->type)
    {
    case REDIRECTING_OUTPUT:
    case CLOBBER:
        redirect_output(word, redirection->io_number);
        break;
    case APPEND_OUTPUT:
        redirect_appending_output(word, redirection->io_number);
        break;
    case REDIRECTING_INPUT:
        redirect_input(word, redirection->io_number);
        break;
    case OPEN_FD:
        redirect_fd(word, redirection->io_number);
        break;
    case DUPLICATE_OUTPUT:
        redirect_duplicating_output(fd_str_to_int(word),
                                    redirection->io_number);
        break;
    case DUPLICATE_INPUT:
        redirect_duplicating_input(fd_str_to_int(word), redirection->io_number);
        break;
    default:
        fprintf(stderr, "redirection not yet implemented!\n");
    }
    free(word);
}
