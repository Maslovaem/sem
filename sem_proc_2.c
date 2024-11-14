#include <sys/shm.h>
#include <stdio.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>

#define SEM_KEY 2007
#define SHM_KEY 2007

#define SHMEM_SIZE      4096

int main()
{
        int shm_id, sem_id;
        char *shm_buf = NULL;
        struct sembuf sem[1];
	int current = 0;

        shm_id = shmget(SHM_KEY, SHMEM_SIZE, 0666);
        if (shm_id == -1) 
        {
                fprintf (stderr, "shmget() error\n");
                return 1;
        }

	sem_id = semget (SEM_KEY, 1, 0600);
        if (sem_id == -1) 
        {
                fprintf (stderr, "semget() error\n");
                return 1;
        }

        shm_buf = (char *)shmat(shm_id, NULL, 0);
        if (shm_buf == (char *)(-1)) 
        {
                fprintf (stderr, "shmat() error\n");
                return 1;
        }
        
	sem[0].sem_num = 0;
	while(1)
        {       
		if(atoi(shm_buf) != 0)
		{
                	sem[0].sem_op = -1;
                	semop(sem_id, sem, 1);;
                	sprintf(shm_buf, "%d", atoi(shm_buf) - 1);
                	printf("proc 2: %d\n", atoi(shm_buf));
                	sleep(1);
                	sem[0].sem_op = 1;
                	semop(sem_id, sem, 1);

                	if((atoi(shm_buf) - 1) <= 0)
                        	break;
		}
        }
        
	printf("sem_id: %d\n", sem_id);
	printf("shm_id: %d\n", shm_id);
	
	semctl(sem_id, 1, IPC_RMID, NULL);
        shmdt(shm_buf);
	shmctl(shm_id, IPC_RMID, NULL);
        return 0;
}
