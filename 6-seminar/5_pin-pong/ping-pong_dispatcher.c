#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define SHMEM_SIZE	4096
#define SHM_KEY		2007

int main (void) {
    int shm_id, sem_id;
	int *shm_buf;
    	
	shm_id = shmget(SHM_KEY, SHMEM_SIZE, IPC_CREAT | 0600); // по одному ключу получаем один айдишник шейрд мема в 1 и 2 игроке и диспетчере
	if (shm_id == -1) {
		fprintf (stderr, "[disp] shmget() error\n");
		return 1;
	}
    fprintf (stderr, "[disp] shm_ID: %d\n", shm_id);

	shm_buf = (int *)shmat(shm_id, NULL, 0); // // подключаем одну разделяемую память в 3 и 4 и дитспетчере
	if (shm_buf == (int *) -1) {
		fprintf (stderr, "[disp] buf by shmat() error\n");
		return 1;
	}

	shm_buf[2] = 0; //пока никто не записал число в шейрд мем
	shm_buf[3] = 666; //пока положим что-то
    while (1) {
		if (shm_buf[3] == 0) { // дошли до 0 в игре
			fprintf(stderr, "zero time! => finishing...\n");
			shmdt(shm_buf); // отсоединяем
			shmctl(shm_id, IPC_RMID, NULL); // удаляем шейрд мемори
			break;
		}
		fprintf(stderr, "state of shm: pid scan 1: %d, pid scan 2: %d\n", shm_buf[0], shm_buf[1]);
        sleep(2);
    }
    return 0;
}
