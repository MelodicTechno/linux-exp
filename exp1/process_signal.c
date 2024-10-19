#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <fcntl.h>

void child_signal_handler(int sig) {
    if (sig == SIGUSR1) {
        printf("Child process 1 is killed by parent!!\n");
        exit(0);
    } else if (sig == SIGUSR2) {
        printf("Child process 2 is killed by parent!!\n");
        exit(0);
    }
}

int kbhit() {
    struct termios oldt, newt;
    int oldf;
    char ch;
    int result;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    result = read(STDIN_FILENO, &ch, 1);
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    return (result > 0) ? ch : 0;
}

int main() {
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

    printf("Press 'Enter' key to kill the child processes...\n");
    while (1) {
        int key = kbhit();
        if (key == '\n') { // Check for Enter key
            printf("Parent process received Enter key. Sending signals...\n");
            kill(pid1, SIGUSR1); // Send SIGUSR1 to first child
            kill(pid2, SIGUSR2); // Send SIGUSR2 to second child
            break;
        }
    }

    // Wait for child processes to finish
    wait(NULL);
    wait(NULL);
    printf("Parent process is killed!!\n");

    return 0;
}
