//Process1.c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main(){
	pid_t pid = fork();

	if (pid < 0){
		fprintf(stderr, "FORK failed\n");
		return 1;
	}
	else if (pid == 0){
		// added these debug prints to verify execution
		printf("child process executing process2 at %d\n", getpid());
		execlp("./question2_process2", "question2_process2", NULL);
		return 1;
	}
	else{
		// process1 code
		int count = 0;
		printf("parent process1 started at %d\n", getpid());

		while (1) {
			if (count % 3 == 0){
				printf("cycle number: %d - %d is a multiple of 3\n", count, count);
			}
			else {
				printf("cycle number: %d\n", count);
			}
			count++;
			sleep(1);
		}
	}
	
	return 0;
}	