#include <sys/shm.h>
#include <stdio.h>
#include <sys/sem.h>

#define SEM_KEY 2007
#define SHM_KEY 2007
// Написать комментарии, отладить работу

int main (int argc, char ** argv) {
	int shm_id, sem_id;
	char * shm_buf;
	struct sembuf sb[1];

	shm_id = shmget(SHM_KEY, 1, 0600); // по одному ключу создаём один айдишник шейрд мема в 3 и 4
	if (shm_id == -1) {
		fprintf (stderr, "shmget() error\n");
		return 1;
	}
	
	sem_id = semget (SEM_KEY, 1, 0600); // по одному ключу в 4й получаем созданный 3й набор семафоров
	if (sem_id == -1) {
		fprintf (stderr, "semget() error\n");
		return 1;
	}

	shm_buf = (char *)shmat(shm_id, 0, 0); // подключаем одну разделяемую память в 3 и 4
	if (shm_buf == (char *) -1) 
	{
		fprintf (stderr, "shmat() error\n");
		return 1;
	}

	printf ("[4] SMS from 3.c: %s\n", shm_buf);

	sb[0].sem_num = 0;
	sb[0].sem_flg = SEM_UNDO;
	sb[0].sem_op = 1; // 4я для 0 семафора будет увеличивать
	semop (sem_id, sb, 1); // разблокируем 3ю для очистки
	
	shmdt (shm_buf);

	return 0;
}