//Process1.c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

struct {
    int multiple;
    int count;
} typedef shared_data;

int main(){
    key_t key = ftok("question4_process1.c", 'A');
    if (key == -1) {
        perror("ftok failed");
        return 1;
    }

    int shmid = shmget(key, sizeof(shared_data), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }

    shared_data *data = (shared_data *) shmat(shmid, NULL, 0);
    if( data == (void *) -1) {
        perror("shmat failed");
        return 1;
    }

    data->multiple = 3;
    data->count = 0;
	printf("shared memory created with id %d\n", shmid);
    printf("muliple set to %d\n", data->multiple);

    pid_t pid = fork();
	if (pid < 0){
		fprintf(stderr, "FORK failed\n");
        shmdt(data);
        shmctl(shmid, IPC_RMID, NULL);
        return 1;		
	}
	else if (pid == 0){
        //convert shmid to string to pass as argument to process 2
        char shmid_string[20];
        sprintf(shmid_string, "%d", shmid);

        // added these debug prints to verify execution
		printf("child process executing process2 at %d\n", getpid());

		execlp("./question4_process2", "question4_process2", shmid_string, NULL);

        // in case we reach here, execlp failed
        perror("execlp failed");
		return 1;
	}
	else{
		// process1 code
		printf("parent process1 started at %d\n", getpid());

		while (data->count <= 500) {
            data->count++;

			if (data->count % data->multiple == 0){
				printf("cycle number: %d - %d is a multiple of %d\n", data->count, data->count, data->multiple);
			}
			else {
				printf("cycle number: %d\n", data->count);
			}

			sleep(1);
		}
        printf("parent process1 completed %d > 500 cycles, waiting for child process2 to finish\n", data->count);
        wait(NULL);
        shmdt(data);
        shmctl(shmid, IPC_RMID, NULL);
        printf("shared memory removed, exiting process1\n");

	}
	
	return 0;
}	