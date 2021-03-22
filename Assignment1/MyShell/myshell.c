#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <linux/limits.h>

#define MAX_BUFFER 1024        // maximum line buffer
#define MAX_ARGS 64            // maximum number of arguments

// colors for using in output
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define COLOR_RESET  "\033[0m"

char curr_path[MAX_BUFFER];         // current path
char shell_path[MAX_BUFFER];        // shell path

extern char **environ;             // environment variables

/* Convenience macro to silence compiler warnings about unused function parameters. */
#define unused __attribute__((unused))

/* tokanizes entered command line and fill all arguments in argv*/
void tokanize(char* line, char** argv)
{
    char delim[] = " \t\r\n";   // array of all possible delimiters
    int tok_n = 0;
    // tokenize input line on delimeters in delim array
    char* token = strtok(line, delim);
    while (token != NULL)
    {
        if (tok_n+1 <= MAX_ARGS)
        {
            argv[tok_n] = token;
            tok_n++;
            token = strtok(NULL, delim);
        }
        // if number of arguments are more than capacity (MAX_ARGS)
        else
        {
            printf("WARNING: number of arguments in the entered command are out of max range (%d).\n\
            Some arguments might be dropped during execution.\n", MAX_ARGS);
            break;
        }
    }
    argv[tok_n] = NULL; // set last argument to NULL

    // for debugging
    /* int i = 0;
    while (argv[i] != NULL)
    {
        printf("**%s**\n", argv[i]);
        i++;
    } */
}

int error_return(char* error_msg)
{
    fprintf(stderr, ANSI_COLOR_RED "MYSHELL: %s" ANSI_COLOR_RESET "\n", error_msg);
    return 1;
}

/* take shell command input from user */
void readLine(char* line)
{
    if (fgets(line, MAX_BUFFER+1, stdin) == NULL)
    {
        if (feof(stdin))
        {
            printf(ANSI_COLOR_MAGENTA "Bye!\n" ANSI_COLOR_RESET);
            exit(0);
        }
        error_return("command input: Too long input, out of buffer range.");
        return;
    }
}

int checkIfDirExists(char* dirPath)
{
    DIR *dir = opendir(dirPath);
    if (dir)
    {
        closedir(dir);
        return 1;
    }
    else
    {
        return 0;
    }
}

void dirtree(char *basePath, const int root)
{
    int i;
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir)
    {
        // perror("SHELL: dir: ");
        return;
    }

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            for (i=0; i<root; i++) 
            {
                if (i%2 == 0)
                    printf("%c", '|');
                else
                    printf(" ");

            }

            if (dp->d_type != DT_DIR)
            {
                printf("|-- " ANSI_COLOR_BLUE "%s \n" ANSI_COLOR_RESET, dp->d_name);
            }
            else
            {
                printf("|-- " ANSI_COLOR_GREEN "%s\n" ANSI_COLOR_RESET, dp->d_name);
            }

            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            dirtree(path, root + 2);
        }
    }
    // printf("***hello***\n");
    closedir(dir);
}

/* Built-in shell functions */
int cmd_exit(char** argv);
int cmd_help(char** argv);
int cmd_clear(char** argv);
int cmd_echo(char** argv);
int cmd_pause(char** argv);
int cmd_history(char** argv);
int cmd_cd(char** argv);
int cmd_environ(char** argv);
int cmd_dir(char** argv);

/* Built-in command functions take token array and return int */
typedef int cmd_fun_t(char** argv);

/* Built-in command struct and lookup table */
typedef struct fun_desc 
{
  cmd_fun_t *fun;
  char *cmd;
  char *doc;
} fun_desc_t;

fun_desc_t cmd_table[] = 
{
    {cmd_help, "help", "Show the help menu\n"},
    {cmd_exit, "quit", "Exit the command shell\n"},
    {cmd_clear, "clr", "Clear the console screen\n"},
    {cmd_echo, "echo", "echo <comment> will display <comment> on the display followed by a new line. \
Multiple spaces/tabs will be reduced to a single space.\n"},
    {cmd_pause, "pause", "Pause operations of the shell until ‘Enter’ is pressed.\n"},
    {cmd_history, "history", "Display the list of previously executed commands, even on shell restart.\n"},
    {cmd_cd, "cd", "cd <directory> changes the current default directory to <directory>. \
If the <directory> argument is not present, report the current directory. \
If the directory doesn’t exist an appropriate error will  be reported. \
This command will change the PWD environment variable for current shell.\n"},
    {cmd_environ, "environ", "List all the environment strings of the current shell and the bash shell\n"},
    {cmd_dir, "dir", "dir <directory> - Lists all the contents of the directory <directory>\n"}
};

/* Prints a helpful description for the given command */
int cmd_help(unused char** argv) 
{
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    printf("%s - %s\n", cmd_table[i].cmd, cmd_table[i].doc);
  return 0;
}

/* Exits this shell */
int cmd_exit(unused char** argv) 
{
    printf(ANSI_COLOR_MAGENTA "Bye!\n" ANSI_COLOR_RESET);
    exit(0);
}

/* Clear the console screen */
int cmd_clear(unused char** argv)
{
    printf("\e[1;1H\e[2J\n");
    return 0;
}

/* Echo the comment */
int cmd_echo(unused char** argv)
{
    int i=1;
    while (argv[i] != NULL)
    {
        printf(ANSI_COLOR_MAGENTA "%s ", argv[i]);
        i++;
    }
    printf(ANSI_COLOR_RESET "\n");
}

/* Pause operations of the shell until ‘Enter’ is pressed */
int cmd_pause(unused char** argv)
{
    getpass("Press ENTER to continue...");
    return 0;
}

/* Display the list of previously executed commands, even on shell restart */
int cmd_history(unused char** argv)
{
    FILE *fptr;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    char hist_path[MAX_BUFFER];

    strcpy(hist_path, getenv("shell"));
    dirname(hist_path);
    strcat(hist_path, "/.myshell_history");

    fptr = fopen(hist_path, "r");

    while ((read = getline(&line, &len, fptr)) != -1) 
    {
        printf("%s", line);
    }

    fclose(fptr);
    if (line)
        free(line);
    
    return 0;
}

/* Change directory */
int cmd_cd(unused char** argv)
{
    // if no arguments are given just print current working directory
    if (argv[1] == NULL)
    {
        printf("%s\n", getenv("PWD"));
        return 0;
    }
    // if 1 argument is given - try to change to that directory
    else if (argv[2] == NULL)
    {
        // if change fails, return corrosponding error
        if (chdir(argv[1]) == -1)
        {
            perror(ANSI_COLOR_RED "myshell: cd ");
            return 1;
        }
        // if change is successful, change environment variable PWD to changed directory path
        else
        {
            char buff[MAX_BUFFER];
            getcwd(buff, MAX_BUFFER);
            setenv("PWD", buff, 1);
            return 0;
        }
    }
    // if more than 1 arguments return error
    else
    {
        error_return("cd: Too many arguments.");
        return 1;
    }
}

/* List all the environment strings of the current shell and the bash shell */
int cmd_environ(unused char** argv)
{
    char** env = environ;
    while (*env != NULL)
    {
        printf("%s\n\n", *env);
        env++;
    }
}

/* List all the contents of the directory */
int cmd_dir(unused char** argv)
{
    if (argv[1] == NULL)
    {
        error_return("dir: directory name not provided");
        return 1;
    }
    else if (argv[2] != NULL)
    {
        error_return("dir: too many arguments");
        return 1;
    }
    else
    {
        if (checkIfDirExists(argv[1]))
        {
            dirtree(argv[1], 0);
            return 0;
        }
        else
        {
            perror("MYSHELL: dir: ");
            return 1;
        }
    }
}

/* Saves the list of previously executed commands */
void save_history(char* line)
{
    char hist_path[MAX_BUFFER];
    strcpy(hist_path, getenv("shell"));
    dirname(hist_path);
    strcat(hist_path, "/.myshell_history");
    FILE *fptr;
    fptr = fopen(hist_path, "a");
    fprintf(fptr, "%s", line);
    fclose(fptr);
}

/* Looks up the built-in command, if it exists. */
int lookup(char cmd[]) {
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0))
      return i;
  return -1;
}

int execute(char** argv)
{
    pid_t pid;
    int status;

    // if command is empty
    if (argv[0] == NULL) return 0;
    
    // check if the command is a built-in command
    int cmd_idx = lookup(argv[0]);
    // if it is a built-in command execute it from cmd_table
    if (cmd_idx >= 0) 
    {
        if(cmd_table[cmd_idx].fun(argv)) return 1;
    }
    // else try to execute it using execvp
    else
    {
        // fork a child process
        if ((pid = fork()) < 0)
        {
            error_return("fork: forking child process failed.");
            return 1;
        }
        // for child process - 
        else if (pid == 0)
        {
            // execute the command
            if (execvp(argv[0], argv) < 0)
            {
                error_return("input command/file: No such built-in command or file or directory");
                return 1;
            }
        }
        // for parent process - 
        else
        {
            // wait for command to execute in child process
            while (wait(&status) != pid);
        }
    }

    return 0;
}

/* Executes batch file line-by-line */
void execute_batchfile(char* filepath)
{
    FILE *fptr;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    char* argv[MAX_ARGS+1];

    fptr = fopen(filepath, "r");
    while ((read = getline(&line, &len, fptr)) != -1) 
    {
        tokanize(line, argv);
        // if execute returns 1, then it means there was some eror executing that line in batch file
        // immediately stop execution
        if(execute(argv)) exit(1);
    }

    fclose(fptr);
    if (line)
        free(line);
}


/* get application full path in path */
char * app_path(char * path, const char * argv0)
{
    char buf[PATH_MAX];
    char * pos;
    if (argv0[0] == '/') 
    {    // run with absolute path
        strcpy(buf, argv0);
    } else 
    {    // run with relative path
        if(NULL == getcwd(buf, PATH_MAX)) {
            perror("getcwd error");
            return NULL;
        }
        strcat(buf, "/");
        strcat(buf, argv0);
    }
    if (NULL == realpath(buf, path)) 
    {
        perror("realpath error");
        return NULL;
    }
    return path;
}

void init_shell(const char * argv0)
{
    // set environment 
    app_path(shell_path, argv0);
    printf("shell_path: %s\n", shell_path);
    setenv("shell", shell_path, 1);
    // clear the console
    // printf("\e[1;1H\e[2J");

    // show welcome message to the user
    printf("\n/***\n" ANSI_COLOR_YELLOW "\
    *            _____                _____ _          _ _ \n\
    *           |  __ \\              / ____| |        | | |\n\
    *       __ _| |__) |__ _ _   _  | (___ | |__   ___| | |\n\
    *      / _` |  _  // _` | | | |  \\___ \\| '_ \\ / _ \\ | |\n\
    *     | (_| | | \\ \\ (_| | |_| |  ____) | | | |  __/ | |\n\
    *      \\__, |_|  \\_\\__,_|\\__, | |_____/|_| |_|\\___|_|_|\n\
    *       __/ |             __/ |                        \n\
    *      |___/             |___/                         \n\
    */" ANSI_COLOR_RESET "\n");

    char* username = getenv("USER");
    printf("/***      Welcome " ANSI_COLOR_MAGENTA "%s" ANSI_COLOR_RESET " to our gRay Shell!\n", username);
    printf("/***      Press" ANSI_COLOR_MAGENTA " \"help\"" ANSI_COLOR_RESET " for user manual.\n");
}

// puts shell command prompt string on console
void putShellPrompt()
{
    fprintf(stderr, ANSI_COLOR_YELLOW "\n%s@%s" ANSI_COLOR_CYAN" %s" ANSI_COLOR_RESET " > ",getenv("USER"), getenv("NAME"), getenv("PWD"));
}

int main(int argc, char* argv[])
{
    if (argc > 2)
    {
        error_return("Maximum 1 argument is allowed - batch file path.");
        return 1;
    }
    else if (argc == 2)
    {
        execute_batchfile(argv[1]);
    }
    else
    {
        init_shell(argv[0]);

        char line[MAX_BUFFER+1];     // one extra for terminating null character
        argv = (char**) malloc(sizeof(char*)*(MAX_ARGS+1)); // one extra for terminating null character
        char* prompt;

        while (1)
        {    
            putShellPrompt();
            readLine(line);
            save_history(line);
            tokanize(line, argv);
            execute(argv);
        }
    }

    return 0;
}