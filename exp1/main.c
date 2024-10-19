#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

int main() {
    int pipefd1[2], pipefd2[2];
    pid_t pid1, pid2;
    char buffer[BUFFER_SIZE];

    // Create two pipes
    if (pipe(pipefd1) == -1 || pipe(pipefd2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork first child process
    pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {  // Child process 1
        close(pipefd1[0]);  // Close unused read end
        const char *message1 = "Child process 1 is sending a message!";
        write(pipefd1[1], message1, strlen(message1) + 1);
        close(pipefd1[1]);  // Close write end after sending message
        exit(0);
    }

    // Fork second child process
    pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {  // Child process 2
        close(pipefd2[0]);  // Close unused read end
        const char *message2 = "Child process 2 is sending a message!";
        write(pipefd2[1], message2, strlen(message2) + 1);
        close(pipefd2[1]);  // Close write end after sending message
        exit(0);
    }

    // Parent process
    close(pipefd1[1]);  // Close unused write end of pipe 1
    close(pipefd2[1]);  // Close unused write end of pipe 2

    // Read messages from both pipes
    read(pipefd1[0], buffer, BUFFER_SIZE);
    printf("Parent received: %s\n", buffer);
    read(pipefd2[0], buffer, BUFFER_SIZE);
    printf("Parent received: %s\n", buffer);

    // Close read ends
    close(pipefd1[0]);
    close(pipefd2[0]);

    // Wait for both children to finish
    wait(NULL);
    wait(NULL);

    return 0;
}
