/* liam beckman */
/* 18 may 2018 */
/* cs344: progream 3 */
/* https://www.w3resource.com/c-programming-exercises/string/c-string-exercise-31.php */
/* https://github.com/angrave/SystemProgramming/wiki/Forking,-Part-2:-Fork,-Exec,-Wait */
/* https://stackoverflow.com/questions/2595503/determine-pid-of-terminated-process */
/* https://stackoverflow.com/questions/190229/where-is-the-itoa-function-in-linux */
/* https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/ */
/* https://www.poetryfoundation.org/poems/51021/the-sea-shell */

/* define sigaction structs */
#define _XOPEN_SOURCE

/* getline */
#define _GNU_SOURCE

/* file writing and reading */
#include <fcntl.h>
/* signals */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
/* waitpid() */
#include <sys/wait.h>
/* process id's */
#include <unistd.h>
#include <string.h>

/* Your shell must support command lines with a maximum length of 2048 characters, and a maximum of 512 arguments. */
#define MAX_ARG 512
#define MAX_CHAR 2048


/* function prototypes */
void path(char **args, int argCount);
void execute(char** args, int argCount, int pidShell);
void input(char* infile);
void output(char* outfile);
char* dollarToPid(char* args, int pidShell);

void catchSIGTSTP(int signo);
void catchSIGCHLD(int signo);

int getFgOnly();
void setFgOnly(int newValue);

int getBgStatus();
void setBgStatus(int newValue);

int getWrite();
void setWrite(char* newString);

/* global variables */
/* all commands will be in foreground */
int fgOnly = 0;

/* command will run in background */
int bgStatus = 0;

/* status of background processes before prompt */
char writeBuffer[MAX_CHAR];

/* status of background processes before prompt */
char statusBuffer[MAX_CHAR];



int main(int argc, char* argv[])
{
    /* SIGTSTP: ctrl z */
    struct sigaction SIGTSTP_action = {0};
    SIGTSTP_action.sa_handler = catchSIGTSTP;
    sigfillset(&SIGTSTP_action.sa_mask);
    SIGTSTP_action.sa_flags = 0;

    sigaction(SIGTSTP, &SIGTSTP_action, NULL);

    /* ignore SIGINT     */
    struct sigaction ignore_action = {0};
    ignore_action.sa_handler = SIG_IGN;
    sigaction(SIGINT, &ignore_action, NULL);

    int numCharsEntered = -5;
    size_t bufferSize = 0;
    char* lineEntered = NULL;

    memset(writeBuffer, '\0', sizeof(writeBuffer));

    while (1)
    {
        /* if there are messages in the global write buffer */
        /* output status of background process */
        printf("%s", writeBuffer);
        fflush(stdout);

        /* reset writeBuffer */
        writeBuffer[0] = '\0';

        while (1)
        {

            if (getWrite() == 0)
            {
                /* prompt symbol */
                printf(":");
                fflush(stdout);
            }

            numCharsEntered = getline(&lineEntered, &bufferSize, stdin);
            if (numCharsEntered == -1)
            {
                clearerr(stdin);
            }

            else
            {
                break;
            }
        }

        /* Your shell must support command lines with a maximum length of 2048 characters, and a maximum of 512 arguments. */
        char inputString[MAX_ARG][MAX_CHAR];

        int m;
        for (m = 0; m < MAX_ARG; m++)
        {
            memset(inputString[m], '\0', strlen(inputString[m]));
        }

        int j = 0;
        int count = 0;
        char *args[MAX_ARG];

        /* remove new line from end of input */
        lineEntered[strcspn(lineEntered, "\n")] = '\0';

        /* break input into individual words in inputString array */
        int i;
        for (i = 0; i <= (strlen(lineEntered)); i++)
        {
            if (lineEntered[i] == ' ' || lineEntered[i] == '\0')
            {
                inputString[count][j] = '\0';
                args[count] = inputString[count];
                /*printf("args[count]: %s\n", args[count]); */
                count += 1;
                j = 0;
            }
            else
            {
                inputString[count][j] = lineEntered[i];
                j += 1;
            }
        }


        /* exit command () */
        if (strcmp(inputString[0], "exit") == 0)
        {
            /* kill all ongoing child processes */
            pid_t pid;
            int childExitStatus;
            while ((pid = waitpid(-1, &childExitStatus, WNOHANG)) != -1)
            {
                kill(pid, SIGINT);
            }

            /* exit program */
            exit(0);
        }

        /* cd command (change directory, defaults to user's home directory with no arguments) */
        char *directory;
        if (strcmp(inputString[0], "cd") == 0)
        {
            /* if no directory was specified */
            if (inputString[1][0] == '\0')
            {
                /* change to the user's home directory */
                directory = getenv("HOME");
            }

            /* else a directory was specified */
            else
            {
                /* so change to that directory */
                directory = inputString[1];

                /* replace $$ with pid of current process */
                if (strstr(directory, "$$") != 0)
                {
                    /* get the pid of the shell */
                    int pidShell = getpid();

                    directory = dollarToPid(directory, pidShell);
                }

                char prefix[3];
                memset(prefix, '\0', strlen(prefix));

                strcpy(prefix, "./");
                strcat(prefix, directory);

            }

            if (chdir(directory) != 0)
            {
                perror("chdir error");
            }
        }

        /* status command () */
        else if (strcmp(inputString[0], "status") == 0)
        {
            /* exit status of last foreground command */
            printf(statusBuffer);
        }

        /* ignore comment lines or blank lines */
        else if (strcmp(inputString[0], "#") == 0 ||
                 strcmp(inputString[0], "\0") == 0)
        {
            continue;
        }

        /* command not built in, parse user's path */
        else
        {
            path(args, count);
        }

        /* free lineEntered pointer */
        free(lineEntered);
        lineEntered = NULL;
    }
}


/* stdout redirection ">" */
void output(char* outfile)
{
    /* open output file for writing */
    /*printf("outfile: %s\n", outfile); */
    int targetFD = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (targetFD == -1)
    {
        perror("open() error");
        exit(1);
    }

    /* redirect stdout to the output file */
    int result = dup2(targetFD, 1);
    if (result == -1)
    {
        perror("dup2 error");
        exit(1);
    }

    close(targetFD);
}


/* stdin redirection "<" */
void input(char* infile)
{
    /* open input file for reading */
    int targetFD = open(infile, O_RDONLY);

    /* close on exec */
    fcntl(targetFD, F_SETFD, FD_CLOEXEC);
    if (targetFD == -1)
    {
        perror("open() error");
        exit(1);
    }

    /* redirect stdin to the input file */
    int result = dup2(targetFD, 0);
    if (result == -1)
    {
        perror("dup2 error");
        exit(1);
    }

    close(targetFD);
}


void execute(char** args, int argCount, int pidShell)
{
    char *myargs[argCount + 1];
    memset(myargs, '\0', sizeof(myargs));
    int execArgs = 0;
    int redirected = 0;

    /* default SIGINT action */
    struct sigaction SIGINT_action = {0};
    SIGINT_action.sa_handler = SIG_DFL;
    sigaction(SIGINT, &SIGINT_action, NULL);

    /*printf("execute argCount: %d\n", argCount); */
    int k;
    for (k = 0; k < argCount; k++)
    {
        /*printf("args[%d]: %s\n", k, args[k]); */
        if (strcmp(args[k], ">") == 0)
        {
            output(args[k + 1]);
            redirected = 1;
        }

        else if (strcmp(args[k], "<") == 0)
        {
            input(args[k + 1]);
            redirected = 1;
        }

        /* don't try to exec with ampersand */
        else if (strcmp(args[k], "&") == 0)
        {
            continue;
        }

        else if (!redirected)
        {
            /* replace $$ with pid of current process */
            if (strstr(args[k], "$$") != 0)
            {
                args[k] = dollarToPid(args[k], pidShell);
            }

            /* add arguments to array that will be passed to execvp(). */
            /*printf("adding %s to myargs\n", args[k]); */
            myargs[execArgs] = args[k];
            execArgs += 1;
        }

    }

    myargs[argCount] = NULL;

     /* If a command fails because the shell could not find the command to run, then the shell will print an error message and set the exit status to 1. */
    if (execvp(*myargs, myargs) != 0)
    {
        perror("exec() error");
        exit(2);
    }
}


/* change "$$" to pid of shell */
char* dollarToPid(char* args, int pidShell)
{
    /* all characters before $$ */
    char* prefix = strchr(args, '$');

    /* all characters following $$ */
    char* token = strtok(prefix, "$");
    char* suffix = (char *) malloc((sizeof(token)+ 1) * sizeof(char));

    /* if there are characters following $$, */
    /* make a copy in the suffix variable */
    if (token != NULL)
    {
        strcpy(suffix, token);
    }

    /* change $$ to pid of shell */
    /* sprintf is standard, could also use */
    /* itoa(getpid()) here */
    sprintf(prefix, "%d", pidShell);

    /* if there were characters following $$ in the original */
    /* string, append them to the final pid string */
    if (suffix != NULL)
    {
        strcat(prefix, suffix);
    }
    /*printf("$$ :: %s\n", prefix); */

    /* TODO maybe not necessary */
    free(suffix);

    return args;
}


void path(char **args, int argCount)
{
    pid_t spawnPid = -5;
    int pidShell = getpid();

    /* set to background if last character is & */
    if (strcmp(args[argCount - 1], "&") == 0 && getFgOnly() == 0)
    {
        /*setFgOnly(0); */
        /*printf("setting to bg\n"); */
        setBgStatus(1);
    }

    /* otherwise make sure process runs in foreground */
    else
    {
        /*printf("setting to fg\n"); */
        setBgStatus(0);
    }


    /* register signal handler if command is in background */
    /* and register before child can complete */
    if (getBgStatus() == 1)
    {
        struct sigaction SIGCHLD_action = {0};

        SIGCHLD_action.sa_handler = catchSIGCHLD;
        sigfillset(&SIGCHLD_action.sa_mask);
        SIGCHLD_action.sa_flags = 0;
        sigaction(SIGCHLD, &SIGCHLD_action, NULL);
    }

    /* fork child process */
    spawnPid = fork();
    switch (spawnPid)
    {
        /* error in forking child process */
        case -1:
            perror("fork() error\n");
            exit(1);
            break;

        /* child process */
        case 0:
            execute(args, argCount, pidShell);
            break;

        /* parent process */
        default:
        {
            int childExitStatus;

            /* if the process is running in the foreground */
            if (getBgStatus() == 0)
            {
                /* block until the process is done */
                if (waitpid(spawnPid, &childExitStatus, 0) > 0)
                {

                    /* processes exited */
                    if (WIFEXITED(childExitStatus) != 0)
                    {
                            int exitStatus = WEXITSTATUS(childExitStatus);

                            /* write exit status to status buffer (for "status" command) */
                            sprintf(statusBuffer, "exit value %d\n", exitStatus);
                    }

                    /* process terminated by signal */
                    else if (WIFSIGNALED(childExitStatus) != 0)
                    {
                        int termSignal = WTERMSIG(childExitStatus);
                        if (termSignal == 123)
                        {
                            /*termSignal = 2; */
                        }

                        /* write signal to status buffer (for "status" command) */
                        sprintf(statusBuffer, "terminated by signal %d\n", termSignal);
                        sprintf(writeBuffer, "terminated by signal %d\n", termSignal);
                        /*char message[512]; */
                        /*sprintf(message, "terminated by signal %d\n", termSignal); */
                        /*write(STDOUT_FILENO, message, 36); */
                        /*fflush(stdout); */
                    }
                }
            }

            /* if the process is running in the background */
            if (getBgStatus() == 1)
            {
                sprintf(writeBuffer, "background pid is %d\n", spawnPid);
                /* do not block, just run in background */
                waitpid(-1, &childExitStatus, WNOHANG);
            }


            break;
        }
    }
}


/* For our purposes, note that the printf() family of functions is NOT re-entrant. In your signal handlers, when outputting text, you must use other output functions! */

void catchSIGTSTP(int signo)
{
    if (getFgOnly() == 0)
    {
        /*char *message = "Entering foreground-only mode (& is now ignored)\n"; */
        /*write(STDOUT_FILENO, message, 81); */
        char *message = "\nEntering foreground-only mode (& is now ignored)\n";
        write(STDOUT_FILENO, message, 51);
        fflush(stdout);
        setFgOnly(1);
    }

    else
    {
        /*char *message = "Exiting foreground-only mode\n"; */
        /*write(STDOUT_FILENO, message, 38); */
        char *message = "\nExiting foreground-only mode\n";
        write(STDOUT_FILENO, message, 31);
        fflush(stdout);
        setFgOnly(0);
    }
}

void catchSIGCHLD(int signo)
{
    /*printf("SIGCHLD recieved :: bgStatus -> %d\n", bgStatus); */
    pid_t spawnPid;
    int childExitStatus;
    spawnPid = waitpid(-1, &childExitStatus, WNOHANG);

    /*if the pid is not from a background process, output info */
    if (spawnPid > 0)
    {
        sprintf(writeBuffer, "background pid %d is done: ", spawnPid);

        if (WIFEXITED(childExitStatus) != 0)
        {
            int exitStatus = WEXITSTATUS(childExitStatus);

            sprintf(writeBuffer + strlen(writeBuffer), "exit value %d\n", exitStatus);
        }

        else if (WIFSIGNALED(childExitStatus) != 0)
        {
            int termSignal = WTERMSIG(childExitStatus);

            sprintf(writeBuffer + strlen(writeBuffer), "terminated by signal %d\n", termSignal);

        }
    }

    /*setWrite(writeBuffer); */
}

int getWrite()
{
    /* if writeBuffer is empty, return 0 */
    if(writeBuffer[0] == '\0')
    {
        return 0;
    }

    /* else if writeBuffer has contents, return 1 */
    return 1;
}

void setWrite(char* newString)
{
    strcat(writeBuffer, newString);
}

int getFgOnly()
{
    return fgOnly;
}

void setFgOnly(int newValue)
{
fgOnly = newValue;
}

int getBgStatus()
{
return bgStatus;
}

void setBgStatus(int newValue)
{
bgStatus = newValue;
}


/*
~ * ~ * ~ * ~ * ~ * ~ * ~ * ~

The Sea Shell
By Marin Sorescu
Translated by Michael Hamburger

~ * ~ * ~ * ~ * ~ * ~ * ~ * ~

I have hidden inside a sea shell
but forgotten in which.

Now daily I dive,
filtering the sea through my fingers,
to find myself.
Sometimes I think
a giant fish has swallowed me.
Looking for it everywhere I want to make sure
it will get me completely.

The sea-bed attracts me, and
I'm repelled by millions
of sea shells that all look alike.
Help, I am one of them.
If only I knew, which.

How often I've gone straight up
to one of them, saying: That’s me.
Only, when I prised it open
it was empty.

~ * ~ * ~ * ~ * ~ * ~ * ~ * ~
*/
