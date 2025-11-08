//Process2.c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
	int count = 0;
    // added these debug prints to verify execution
    printf("child process2 started at %d\n", getpid());
	while (count >= -500) {
		if (count % 3 == 0){
            printf("P2 - "); // added these debug prints to verify execution
			printf("cycle number: %d - %d is a multiple of 3\n", count, count);
		}
        else {
            printf("P2 - "); // added these debug prints to verify execution 
            printf("cycle number: %d\n", count);
        }
		count--;
		sleep(1);
	}
	return 0;
}