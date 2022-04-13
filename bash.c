#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

/* prompt setter */
// type_prompt {done}

/* builtin commands */
// exit {done}
// pid
// ppid
// cd {done}
// pwd {done}
// set
// get

/* takes a command from user */
// read_command

int main(int argc, char *argv[])
{
    // prompt printing function
    void type_prompt(char *wd)
    {
        if (argc == 3)
        {
            if (!(strcmp(argv[1], "-p")))
            {
                char *prompt = argv[2];
                printf("\033[38;5;82;1m%s\033[0m:\033[38;5;45m%s\033[38;5;231m$ \033[0m", prompt, wd);
            }
        }
        else if (argc == 1)
        {
            printf("\033[38;5;82;1m%s\033[0m:\033[38;5;45m%s\033[38;5;231m$ \033[0m", "451sh", wd);
        }
        else
        {
            printf("error: 2 argument expected but %d was given {-p <prompt>} \n", argc - 1);
        }
    }

    // current working directory
    char *pwd_command()
    {
        char *cwd = (char *)calloc(512, sizeof(char));

        getcwd(cwd, 512);
        // printf("%s\n", cwd);
        return cwd;
    }

    // command reader, returns an array of commands & args(command is 1st element)
    char **read_command()
    {
        char string[30]; /* initially input consist of 30 char but you need to change it to be dynamic */
        scanf(" %[^\n]s", string);
        char *token = strtok(string, " "), **string_list = (char **)malloc(10 * sizeof(char)); /* (string_list)initially the bassh accepts 1 command and 9 args */
        for (int index = 0; token != NULL; index++)
        {
            string_list[index] = strdup(token);
            token = strtok(NULL, " ");
        }
        return string_list;
    }

    //  change directory command (returns -1 on fail, zero on success)
    int cd(char *path)
    {
        return chdir(path);
    }

    while (1) /*repeat forever*/
    {
        int status;

        int fork_ret = fork();
        if (fork_ret != 0) /* Parent process */
        {
            /* wait for child to exit */
            waitpid(fork_ret, &status, 0);
            kill(getpid(), SIGTERM);
        }
        else /* Child process */
        {
            char *wd = pwd_command();
            type_prompt(wd); /* display prompt */

            char **commands_args_list = read_command(); /* input from terminal */

            /* bulitin commands execution */
            if (!strcmp(commands_args_list[0], "pwd"))
            {
                printf("%s\n", wd);
            }
            else if (!strcmp(commands_args_list[0], "cd"))
            {
                if (commands_args_list[1] == NULL || !strcmp(commands_args_list[1], "~"))
                {
                    int ret_val = cd(getenv("HOME"));
                    if (ret_val == -1)
                    printf("error: No such file or directory\n");
                }
                else
                {
                    int ret_val = cd(commands_args_list[1]);
                    if (ret_val == -1)
                    printf("error: No such file or directory\n");
                }
                
            }
            else if (!strcmp(commands_args_list[0], "exit"))
            {
                exit(status);
            }

            printf("%d", getpid());
        }
    }

    // while (1) /*repeat forever*/
    // {
    //     int status;

    //     char *wd = pwd_command();
    //     type_prompt(wd); /* display prompt */

    //     char **commands_args_list = read_command(); /* input from terminal */

    //     int fork_ret = fork();
    //     if (fork_ret != 0) /* Parent process */
    //     {
    //         /* wait for child to exit */
    //         waitpid(fork_ret, &status, 0);
    //         wd = pwd_command();
    //         printf("2nddone %s \n", wd);
    //     }
    //     else /* Child process */
    //     {
    //         /* non-bulitin commands execution */
    //         // execl("/bin/pwd", "ls", NULL); /* execute non-bulitin commands */

    //         /* bulitin commands execution */
    //         if (!strcmp(commands_args_list[0], "pwd"))
    //         {
    //             printf("%s\n", wd);
    //         }
    //         else if (!strcmp(commands_args_list[0], "cd"))
    //         {
    //             cd("test");
    //             wd = pwd_command();
    //             printf("1stdone %s \n", wd);
    //         }
    //         kill(getpid(), SIGTERM);
    //     }
    // }

    return 0;
}