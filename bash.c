#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/* bulitin commands section */
    // prompt printing function


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

/* non-bulitin commands */
pid_t spawn_ch_process(const char* program, char** arg_list) {
    pid_t ch_pid = fork();
    if (ch_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (ch_pid > 0) {
        printf("\033[38;2;226;125;95;1m** \033[0mspawn child with pid - %d\033[38;2;226;125;95;1m **\033[0m\n", ch_pid);
        return ch_pid;
    } else {
        execvp(program, arg_list);
        perror("execve");
        exit(EXIT_FAILURE);
    }
}

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
        // jobs
        // ">" redirect output
        // clear {done}
        // unset {done}

/* non-builtin commands */
    // execvp()

int main(int argc, char *argv[])
{
    void type_prompt(char *wd) {
        if (argc == 3)
        {
            if (!(strcmp(argv[1], "-p")))
            {
                char *prompt = argv[2];
                printf("\033[38;2;220;50;47;1m%s\033[0m:\033[38;2;38;139;210;1m%s\033[0m$ ", prompt, wd);
            }
        }
        else if (argc == 1)
        {
            printf("\033[38;2;220;50;47;1m%s\033[0m:\033[38;2;38;139;210;1m%s\033[0m$ ", "451sh", wd);
        }
        else
        {
            printf("error: 2 argument expected but %d was given {-p <prompt>} \n", argc - 1);
        }
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
        pid_t child;
        int status;

        child = spawn_ch_process(commands_args_list[0], commands_args_list);

        if (waitpid(child, &status, WUNTRACED | WCONTINUED) == -1) {
            // only executed in case of execve faliur to print the error
            perror("waitpid");

            exit(EXIT_FAILURE);
        }
        if ( WIFEXITED(status) ) {
            int es = WEXITSTATUS(status);
            printf("\033[38;2;226;125;95;1m** \033[0mExit status was %d\033[38;2;226;125;95;1m **\033[0m\n", es);
        }
    }

    return 0;
}
