#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

volatile sig_atomic_t child_terminated = 0;

void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("Parent process received SIGINT\n");
        // Send signals to child processes
        kill(0, SIGUSR1); // Send SIGUSR1 to all child processes
        kill(0, SIGUSR2); // Send SIGUSR2 to all child processes
    }
}

void child_signal_handler(int sig) {
    if (sig == SIGUSR1) {
        printf("Child process 1 is killed by parent!!\n");
        exit(0);
    } else if (sig == SIGUSR2) {
        printf("Child process 2 is killed by parent!!\n");
        exit(0);
    }
}

int main() {
    signal(SIGINT, signal_handler); // Handle SIGINT in parent

    pid_t pid1 = fork();
    if (pid1 == 0) {
        // First child process
        signal(SIGUSR1, child_signal_handler);
        while (1) pause(); // Wait for signals
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Second child process
        signal(SIGUSR2, child_signal_handler);
        while (1) pause(); // Wait for signals
    }

    // Parent process waits for child processes to terminate
    while (child_terminated < 2) {
        // Wait for children to terminate
        int status;
        pid_t child_pid = wait(&status);
        if (child_pid > 0) {
            child_terminated++;
        }
    }

    printf("Parent process is killed!!\n");
    return 0;
}
