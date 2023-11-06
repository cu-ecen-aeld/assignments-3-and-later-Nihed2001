#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

bool do_system(const char *cmd)
{
    return system(cmd) == 0;
}

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char *command[count + 1];
    int i;
    for (i = 0; i < count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;

    int pid = fork();
    int retVal = 0;

    if (pid < 0)
    {
        printf("Error occurred in fork()\n");
        retVal = 1;
    }
    else if (pid == 0)
    {
        // Child process code
        execv(command[0], command);
        // If execv succeeds, this code won't be reached
        perror("execv");
        exit(1); // Exit with an error code
    }
    else
    {
        // Parent process code, wait for the child
        waitpid(pid, &retVal, 0);
    }

    va_end(args);
    return retVal == 0;
}

bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char *command[count + 1];
    int i;
    for (i = 0; i < count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;

    int pid = fork();
    int retVal = 0;
    int fd = open(outputfile, O_WRONLY | O_CREAT, 0666);

    if (pid < 0)
    {
        printf("Error occurred in fork()\n");
        retVal = 1;
    }
    else if (pid == 0)
    {
        // Child process code
        if (dup2(fd, 1) < 0)
        {
            perror("dup2");
            exit(1); // Exit with an error code
        }
        close(fd);

        execv(command[0], command);
        // If execv succeeds, this code won't be reached
        perror("execv");
        exit(1); // Exit with an error code
    }
    else
    {
        // Parent process code, wait for the child
        close(fd);
        waitpid(pid, &retVal, 0);
    }

    va_end(args);
    return retVal == 0;
}

int main()
{
    // Example usage of do_system
    const char *systemCommand = "ls -l";
    if (do_system(systemCommand))
    {
        printf("do_system succeeded.\n");
    }
    else
    {
        printf("do_system failed.\n");
    }

    // Example usage of do_exec
    const char *execCommand = "/bin/ls";
    if (do_exec(2, execCommand, "-l"))
    {
        printf("do_exec succeeded.\n");
    }
    else
    {
        printf("do_exec failed.\n");
    }

    // Example usage of do_exec_redirect
    const char *redirectCommand = "/bin/ls";
    const char *outputFile = "output.txt";
    if (do_exec_redirect(outputFile, 2, redirectCommand, "-l"))
    {
        printf("do_exec_redirect succeeded.\n");
    }
    else
    {
        printf("do_exec_redirect failed.\n");
    }

    return 0;
}
