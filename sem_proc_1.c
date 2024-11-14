#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SHMEM_SIZE      4096

#define SEM_KEY         2007
#define SHM_KEY         2007

union semnum 
{
        int val;
        struct semid_ds *buf;
        unsigned short *array;
} sem_arg;

int main()
{
	int shm_id, sem_id;
        char *shm_buf = NULL;
        int shm_size = 0;
        struct shmid_ds ds;
        struct sembuf sem[1];
        unsigned short sem_vals[1];
	int number = 0;
	int current = 0;

	shm_id = shmget(SHM_KEY, SHMEM_SIZE,
                        IPC_CREAT | IPC_EXCL | 0600);
        if (shm_id == -1) 
        {
                fprintf(stderr, "shmget() error\n");
                return 1;
        }

	shm_buf = (char *)shmat(shm_id, NULL, 0);
        if (shm_buf == (char *)(-1))
        {
                fprintf(stderr, "shmat() error\n");
                return 1;
        }

        shmctl(shm_id, IPC_STAT, &ds);
        
	sem_id = semget(SEM_KEY, 1,
                        0600 | IPC_CREAT | IPC_PRIVATE);
        if (sem_id == -1)
        {
                fprintf(stderr, "semget() error\n");
                return 1;
        }

	sem_vals[0] = 1;
        sem_arg.array = sem_vals;

	if (semctl(sem_id, 0, SETALL, sem_arg) == -1)
        {
                fprintf(stderr, "semctl() error\n");
                return 1;
        }

	sem[0].sem_num = 0;

	scanf("%d", &number);
        sprintf(shm_buf, "%d", number);

        while(1)
	{	
		sem[0].sem_op = -1;
		semop(sem_id, sem, 1);
		sprintf(shm_buf, "%d", atoi(shm_buf) - 1);
		printf("proc 1: %d\n", atoi(shm_buf));
		sleep(1);
		sem[0].sem_op = 1;
		semop(sem_id, sem, 1);

		if((atoi(shm_buf) - 1) <= 0)
			break;
	}

	printf("sem_id: %d\n", sem_id);
	printf("shm_id: %d\n", shm_id);
        
	semctl(sem_id, 1, IPC_RMID, sem_arg);
        shmdt(shm_buf);
        shmctl(shm_id, IPC_RMID, NULL);

	return 0;
}
