//Process1.c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>

struct {
    int multiple;
    int count;
} typedef shared_data;

int main(){
    key_t key = ftok("question5_process1.c", 'A');
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
    printf("multiple set to %d\n", data->multiple);

    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget failed");
        return 1;
    }

    if (semctl(semid, 0, SETVAL, 1) == -1) {
        perror("semctl failed");
        return 1;
    }

    struct sembuf semopbuf_lock;
    semopbuf_lock.sem_num = 0;
    semopbuf_lock.sem_op = -1;
    semopbuf_lock.sem_flg = 0;

    struct sembuf semopbuf_unlock;
    semopbuf_unlock.sem_num = 0;
    semopbuf_unlock.sem_op = 1;
    semopbuf_unlock.sem_flg = 0;

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

        char semid_string[20];
        sprintf(semid_string, "%d", semid);

        // added these debug prints to verify execution
		printf("child process executing process2 at %d\n", getpid());

		execlp("./question5_process2", "question5_process2", shmid_string, semid_string, NULL);

        // in case we reach here, execlp failed
        perror("execlp failed");
		return 1;
	}
	else{
		// process1 code
		printf("parent process1 started at %d\n", getpid());

        while (1) {
            semop(semid, &semopbuf_lock, 1);
            if (data->count > 500) {
                semop(semid, &semopbuf_unlock, 1);
                break;
            }
            data->count++;

			if (data->count % data->multiple == 0){
				printf("cycle number: %d - %d is a multiple of %d\n", data->count, data->count, data->multiple);
			}
			else {
				printf("cycle number: %d\n", data->count);
			}
            semop(semid, &semopbuf_unlock, 1);
			sleep(1);
        }

        printf("parent process1 completed %d > 500 cycles, waiting for child process2 to finish\n", data->count);
        wait(NULL);
        shmdt(data);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID, 0);
        printf("shared memory removed, exiting process1\n");

	}
	
	return 0;
}	