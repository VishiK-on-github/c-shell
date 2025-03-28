#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define SHELL_TOKEN_BUFFER_SIZE 64
#define SHELL_TOKEN_DELIM " \t\r\n\a"

int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);

char *builtin_str[] ={
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &shell_cd,
    &shell_help,
    &shell_exit
};

int shell_num_builtins() {
    return sizeof(builtin_str) / sizeof(char*);
}

int shell_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("shell");
        }
    }
    return 1;
}

int shell_help(char **args) {
    int i;
    printf("Vishu's Shell\n");
    printf("Type program name and args, and press enter.\n");
    printf("built ins:\n");

    for (i = 0; i < shell_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    return 1;
}

int shell_exit(char **args) {
    return 0;
}

char *shell_read_line() {

    char *line = NULL;
    size_t bufsize = 0;

    // we use getline to set the address for the line
    // and set the buffer size from the input stream
    if (getline(&line, &bufsize, stdin) == -1) {

        // check if EOD is there in the input stream
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);
        } else {
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }

    return line;
}

char **shell_split_line(char *line) {

    // init buffer size and array char array index
    int buffer_size = SHELL_TOKEN_BUFFER_SIZE;
    int position = 0;

    // init an array of character pointers and 
    // return pointer to the head of this array
    char **tokens = malloc(buffer_size * sizeof(char*));
    char *token;

    // check if the allocation is of array of char*
    if (!tokens) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    // split the char array based on defined delimiters
    token = strtok(line, SHELL_TOKEN_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        // if position is greater than buffer size
        // increment the buffer size and reallocate the
        // array of char*
        if (position >= buffer_size) {
            buffer_size += SHELL_TOKEN_BUFFER_SIZE;
            tokens = realloc(tokens, buffer_size);

            if (!tokens) {
                fprintf(stderr, "shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        // this is used to get the next token
        token = strtok(NULL, SHELL_TOKEN_DELIM);
    }

    // set position after final to null to signify end
    tokens[position] = NULL;
    return tokens;
}

int shell_launch(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();

    if (pid == 0) {

        if (execvp(args[0], args) == -1) {
            perror("shell");
        }

        exit(EXIT_FAILURE);
    } 
    else if (pid < 0) {
        perror("shell");
    } 
    else {
        
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int shell_exe_line(char **args) {
    int i;

    if (args[0] == NULL) {
        return 1;
    }

    for (i = 0; i < shell_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return shell_launch(args);
}

void shell_loop(void) {
    char *line;
    char **args;
    int status;

    do {
        // print the shell text
        printf("shell> ");
        
        // read the line from console
        line = shell_read_line();

        // split the line from console
        args = shell_split_line(line);

        // execute the command
        status = shell_exe_line(args);

        // cleanup pointers
        free(line);
        free(args);

    } while (status);
}

int main(int argc, char **argv) {

    // event loop
    shell_loop();

    return EXIT_SUCCESS;
}