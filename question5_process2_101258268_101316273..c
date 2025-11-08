//Process2.c
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

int main(int argc, char *argv[]){
    //valdiate if process1 is passing shmid argument for shared memory access
    if(argc != 3){
        fprintf(stderr, "missing %s shmid and semid arguments\n", argv[0]);
        return 1;
    }

    //converts shmid argument back to integer for mem access
    int shmid = atoi(argv[1]);
    if (shmid == -1) {
        perror("atoi failed");
        return 1;
    }

    //converts semid argument back to integer
    int semid = atoi(argv[2]);
    if (semid == -1) {
        perror("atoi failed");
        return 1;
    }

    //attach to existing shared memory
    shared_data *data = (shared_data *) shmat(shmid, NULL, 0);
    if( data == (void *) -1) {
        perror(" process2 shmat failed");
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

    //sleep/wait until process1 has incremented count to at least 100
    while(data->count <= 100){
        sleep(1);
    }

    // added these debug prints to verify execution
    printf(" count is %d, child process2 starting at %d\n", data->count, getpid());

    while (1) {
        semop(semid, &semopbuf_lock, 1);
        if (data->count > 500) {
            semop(semid, &semopbuf_unlock, 1);
            break;
        }
        int current_count = data->count; //since data->count may be accessed by process1
		if (current_count % data->multiple == 0){
            printf("P2 - "); // added these debug prints to verify execution
			printf("cycle number: %d - %d is a multiple of %d\n", current_count, current_count, data->multiple);
		}
        else {
            printf("P2 - "); // added these debug prints to verify execution
            printf("cycle number: %d\n", current_count);
        }
        semop(semid, &semopbuf_unlock, 1); 
		sleep(1);
    }

    printf("child process2 completed %d > 500 cycles child process2 to finish\n", data->count);
    shmdt(data);
	return 0;
}