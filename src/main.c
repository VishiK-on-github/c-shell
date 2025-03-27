#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SHELL_TOKEN_BUFFER_SIZE 64
#define SHELL_TOKEN_DELIM " \t\r\n\a"

char *shell_read_line() {

    char *line = NULL;
    ssize_t bufsize = 0;

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

    int buffer_size = SHELL_TOKEN_BUFFER_SIZE;
    int position = 0;
    char **tokens = malloc(buffer_size * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SHELL_TOKEN_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= buffer_size) {
            buffer_size += SHELL_TOKEN_BUFFER_SIZE;
            tokens = realloc(tokens, buffer_size);

            if (!tokens) {
                fprintf(stderr, "shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, SHELL_TOKEN_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
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