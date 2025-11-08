/**
 * Note: 
 * I compiled and ran this program in the Linux command line.
 * I used the "ps -e" command to find the PID of both processes and
 * typed the "kill" command followed by the PID to kill the two processes
 * one after the other.
 */

#include <stdio.h>
#include <unistd.h>

void process1() {
    printf("Parent process PID: %d\n", getpid());
    int counter = 0;
    while (1) {
        counter++;
        printf("Parent Count: %d\n", counter);
        sleep(2);
    }
}

void process2() {
    printf("Child process PID: %d\n", getpid());
    int counter = 0;
    while (1) {
        counter++;
        printf("Child Count: %d\n", counter);
        sleep(2);
    }
}

int main() {
    /* fork a child process */
    pid_t pid = fork();
    if (pid < 0) { /* error occurred */
        fprintf(stderr, "Fork Failed\n");
        return 1;
    } else if (pid == 0) { /* child process */
        process2();
    } else { /* parent process */
        process1();
    }
    return 0;
}
