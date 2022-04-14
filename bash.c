#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

/* prompt setter */
  // type_prompt {done}

  /* receive a command from user */
      // read_command {done}

/* builtin commands */
    // exit {done}
    // pid {done}
    // ppid {done}
    // cd {done}
    // pwd {done}
    // set {done}
    // get {done}

    /* extra */
        // clear {done}
        // unset {done}

/* non-builtin commands */
    // execvp()

int main(int argc, char *argv[])
{
    /* bulitin commands section */
        // prompt printing function
        void type_prompt(char *wd) {
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
        char *pwd_command() {
            char *cwd = (char *)calloc(512, sizeof(char));
            getcwd(cwd, 512);
            return cwd;
        }

        // command reader, returns an array of commands & args(command is 1st element)
        char **read_command() {
            char string[512]; /* initially input consist of 512 char but you need to change it to be dynamic */
            scanf(" %[^\n]s", string);
            char *token = strtok(string, " "), **string_list = (char **)malloc(10 * sizeof(char)); /* (string_list)initially the bassh accepts 1 command and 9 args */
            for (int index = 0; token != NULL; index++)
            {
                string_list[index] = strdup(token);
                token = strtok(NULL, " ");
            }
            return string_list;
        }

        // change directory command [returns -1 on fail, zero on success]
        int cd(char *path) {
            return chdir(path);
        }

        // [returns current process ID]
        int pid() {
          return getpid();
        }

        // [returns Parent process ID
        int ppid() {
          return getppid();
        }

        // set enviroment variable [returns -1 on fail, zero on success]
        int set(char *var_name, char *val) {
            return setenv(var_name, val, 1);
        }

        char *get(char *var_name){
            return getenv(var_name);
        }

        /* extra */
            void clear() {
                system("clear");
            }

            // unset enviroment variable [returns -1 on fail, zero on success]
            int unset(char *var_name) {
                return unsetenv(var_name);
            }


    while (1) /*repeat forever*/
    {
        char *wd = pwd_command();
        type_prompt(wd); /* display prompt */

        char **commands_args_list = read_command(); /* input from terminal */

        /* bulitin commands execution */
        if (!strcmp(commands_args_list[0], "pwd")) /* pwd command execution(printing) */
        {
            printf("%s\n", wd);
            continue;
        }
        else if (!strcmp(commands_args_list[0], "cd")) /* cd command execution */
        {
            if (commands_args_list[1] == NULL ||
               !strcmp(commands_args_list[1], "~")) /* condition for cd'ing to home directory  */
            {
                int ret_val = cd(getenv("HOME"));
                if (ret_val == -1)
                printf("error: No such file or directory\n");
                continue;
            }
            else
            {
                int ret_val = cd(commands_args_list[1]);
                if (ret_val == -1)
                printf("error: No such file or directory\n");
                continue;
            }

        }
        else if (!strcmp(commands_args_list[0], "pid")) /* printing current process id command */
        {
          printf("%d\n", pid());
          continue;
        }
        else if (!strcmp(commands_args_list[0], "ppid")) /* printing Parent process id command */
        {
          printf("%d\n", ppid());
          continue;
        }
        else if (!strcmp(commands_args_list[0], "set")) /* set enviroment variable */
        {
            if (commands_args_list[1] == NULL) {
                printf("error: at least 1 argument expected but 0 was given {set <var> <value>}\n");
            }
            else if (commands_args_list[2] == NULL) {
                set(commands_args_list[1], "NULL");
            }
            else if (commands_args_list[3] != NULL) {
                printf("bash: set: too many arguments\n");
            }
            else{
                set(commands_args_list[1], commands_args_list[2]);
            }
            continue;
        }
        else if (!strcmp(commands_args_list[0], "unset")) /*  */
        {
            if (commands_args_list[1] == NULL) {
                printf("error: 1 argument expected but 0 was given {unset <var>}\n");
            }
            else{
                unset(commands_args_list[1]);
            }
            continue;
        }
        else if (!strcmp(commands_args_list[0], "get")) /*  */
        {
            if (commands_args_list[1] == NULL) {
                printf("error: 1 argument expected but 0 was given {get <var>}\n");
            }
            else{
                printf("%s\n", get(commands_args_list[1]));
            }
            continue;
        }
        else if (!strcmp(commands_args_list[0], "clear")) /* clear command make your shell cleaner ^_^ */
        {
            clear();
            continue;
        }
        else if (!strcmp(commands_args_list[0], "exit")) /* exit command execution */
        {
            kill(getpid(), SIGTERM);
        }

        /* non-bulitin commands execution */
        int status;
        int fork_ret = fork();
        if (fork_ret != 0) /* Parent process */
        {
            /* wait for child to exit */
            waitpid(fork_ret, &status, 0);
        }
        else /* Child process */
        {
            printf("[%d] %s \n", pid(), commands_args_list[0]);
            // execvp(...);
            printf("Command '%s' not found, please try again\n", commands_args_list[0]);
            exit(status);
        }
    }

    return 0;
}
