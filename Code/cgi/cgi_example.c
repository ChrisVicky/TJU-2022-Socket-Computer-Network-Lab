/*****************************************************************************
 *                                                                           *
 * This file represents a hard-coded CGI driver example.  It will execute a  *
 * file, setup environment variables, and pipe stdin and stdout              *
 * appropriately.                                                            *
 *                                                                           *
 * Authors: Athula Balachandran <abalacha@cs.cmu.edu>                        *
 *          Charles Rang <rang@cs.cmu.edu>                                   *
 *          Wolfgang Richter <wolf@cs.cmu.edu>                               *
 *****************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/**************** BEGIN CONSTANTS ***************/
//#define FILENAME "./cgi_script.py"

#define FILENAME "./cgi_dumper.py"
#define BUF_SIZE 4096

/* note: null terminated arrays (null pointer) */
char* ARGV[] = {
                    FILENAME,
                    NULL
               };

char* ENVP[] = {
                    "CONTENT_LENGTH=",
                    "CONTENT-TYPE=",
                    "GATEWAY_INTERFACE=CGI/1.1",
                    "QUERY_STRING=action=opensearch&search=HT&namespace=0&suggest=",
                    "REMOTE_ADDR=128.2.215.22",
                    "REMOTE_HOST=gs9671.sp.cs.cmu.edu",
                    "REQUEST_METHOD=GET",
                    "SCRIPT_NAME=/w/api.php",
                    "HOST_NAME=en.wikipedia.org",
                    "SERVER_PORT=80",
                    "SERVER_PROTOCOL=HTTP/1.1",
                    "SERVER_SOFTWARE=Liso/1.0",
                    "HTTP_ACCEPT=application/json, text/javascript, */*; q=0.01",
                    "HTTP_REFERER=http://en.wikipedia.org/w/index.php?title=Special%3ASearch&search=test+wikipedia+search",
                    "HTTP_ACCEPT_ENCODING=gzip,deflate,sdch",
                    "HTTP_ACCEPT_LANGUAGE=en-US,en;q=0.8",
                    "HTTP_ACCEPT_CHARSET=ISO-8859-1,utf-8;q=0.7,*;q=0.3",
                    "HTTP_COOKIE=clicktracking-session=v7JnLVqLFpy3bs5hVDdg4Man4F096mQmY; mediaWiki.user.bucket%3Aext.articleFeedback-tracking=8%3Aignore; mediaWiki.user.bucket%3Aext.articleFeedback-options=8%3Ashow; mediaWiki.user.bucket:ext.articleFeedback-tracking=8%3Aignore; mediaWiki.user.bucket:ext.articleFeedback-options=8%3Ashow",
                    "HTTP_USER_AGENT=Mozilla/5.0 (X11; Linux i686) AppleWebKit/535.1 (KHTML, like Gecko) Chrome/14.0.835.186 Safari/535.1",
                    "HTTP_CONNECTION=keep-alive",
                    "HTTP_HOST=en.wikipedia.org",
                    NULL
               };

char* POST_BODY = "This is the stdin body...\n";
/**************** END CONSTANTS ***************/



/**************** BEGIN UTILITY FUNCTIONS ***************/
/* error messages stolen from: http://linux.die.net/man/2/execve */
void execve_error_handler()
{
    switch (errno)
    {
        case E2BIG:
            fprintf(stderr, "The total number of bytes in the environment \
(envp) and argument list (argv) is too large.\n");
            return;
        case EACCES:
            fprintf(stderr, "Execute permission is denied for the file or a \
script or ELF interpreter.\n");
            return;
        case EFAULT:
            fprintf(stderr, "filename points outside your accessible address \
space.\n");
            return;
        case EINVAL:
            fprintf(stderr, "An ELF executable had more than one PT_INTERP \
segment (i.e., tried to name more than one \
interpreter).\n");
            return;
        case EIO:
            fprintf(stderr, "An I/O error occurred.\n");
            return;
        case EISDIR:
            fprintf(stderr, "An ELF interpreter was a directory.\n");
            return;
        case ELIBBAD:
            fprintf(stderr, "An ELF interpreter was not in a recognised \
format.\n");
            return;
        case ELOOP:
            fprintf(stderr, "Too many symbolic links were encountered in \
resolving filename or the name of a script \
or ELF interpreter.\n");
            return;
        case EMFILE:
            fprintf(stderr, "The process has the maximum number of files \
open.\n");
            return;
        case ENAMETOOLONG:
            fprintf(stderr, "filename is too long.\n");
            return;
        case ENFILE:
            fprintf(stderr, "The system limit on the total number of open \
files has been reached.\n");
            return;
        case ENOENT:
            fprintf(stderr, "The file filename or a script or ELF interpreter \
does not exist, or a shared library needed for \
file or interpreter cannot be found.\n");
            return;
        case ENOEXEC:
            fprintf(stderr, "An executable is not in a recognised format, is \
for the wrong architecture, or has some other \
format error that means it cannot be \
executed.\n");
            return;
        case ENOMEM:
            fprintf(stderr, "Insufficient kernel memory was available.\n");
            return;
        case ENOTDIR:
            fprintf(stderr, "A component of the path prefix of filename or a \
script or ELF interpreter is not a directory.\n");
            return;
        case EPERM:
            fprintf(stderr, "The file system is mounted nosuid, the user is \
not the superuser, and the file has an SUID or \
SGID bit set.\n");
            return;
        case ETXTBSY:
            fprintf(stderr, "Executable was open for writing by one or more \
processes.\n");
            return;
        default:
            fprintf(stderr, "Unkown error occurred with execve().\n");
            return;
    }
}
/**************** END UTILITY FUNCTIONS ***************/




int main(int argc, char * argv[])
{
    /*************** BEGIN VARIABLE DECLARATIONS **************/
    pid_t pid;
    int stdin_pipe[2];
    int stdout_pipe[2];
    char buf[BUF_SIZE];
    int readret;
    /*************** END VARIABLE DECLARATIONS **************/

    /*************** BEGIN PIPE **************/
    /* 0 can be read from, 1 can be written to */
    if (pipe(stdin_pipe) < 0)
    {
        fprintf(stderr, "Error piping for stdin.\n");
        return EXIT_FAILURE;
    }

    if (pipe(stdout_pipe) < 0)
    {
        fprintf(stderr, "Error piping for stdout.\n");
        return EXIT_FAILURE;
    }
    /*************** END PIPE **************/

    /*************** BEGIN FORK **************/
    pid = fork();
    /* not good */
    if (pid < 0)
    {
        fprintf(stderr, "Something really bad happened when fork()ing.\n");
        return EXIT_FAILURE;
    }

    /* child, setup environment, execve */
    if (pid == 0)
    {
        /*************** BEGIN EXECVE ****************/
        close(stdout_pipe[0]);
        close(stdin_pipe[1]);
        dup2(stdout_pipe[1], fileno(stdout));
        dup2(stdin_pipe[0], fileno(stdin));
        /* you should probably do something with stderr */

        /* pretty much no matter what, if it returns bad things happened... */
        if (execve(FILENAME, ARGV, ENVP))
        {
            execve_error_handler();
            fprintf(stderr, "Error executing execve syscall.\n");
            return EXIT_FAILURE;
        }
        /*************** END EXECVE ****************/ 
    }

    if (pid > 0)
    {
        fprintf(stdout, "Parent: Heading to select() loop.\n");
        close(stdout_pipe[1]);
        close(stdin_pipe[0]);

        if (write(stdin_pipe[1], POST_BODY, strlen(POST_BODY)) < 0)
        {
            fprintf(stderr, "Error writing to spawned CGI program.\n");
            return EXIT_FAILURE;
        }

        close(stdin_pipe[1]); /* finished writing to spawn */

        /* you want to be looping with select() telling you when to read */
        while((readret = read(stdout_pipe[0], buf, BUF_SIZE-1)) > 0)
        {
            buf[readret] = '\0'; /* nul-terminate string */
            fprintf(stdout, "Got from CGI: BEGIN BUF<<%s>>END OF BUF\n", buf);
        }

        close(stdout_pipe[0]);
        close(stdin_pipe[1]);

        if (readret == 0)
        {
            fprintf(stdout, "CGI spawned process returned with EOF as \
expected.\n");
            return EXIT_SUCCESS;
        }
    }
    /*************** END FORK **************/

    fprintf(stderr, "Process exiting, badly...how did we get here!?\n");
    return EXIT_FAILURE;
}
