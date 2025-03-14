#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHMEM_SIZE	4096
#define SH_MESSAGE	"Hello World!\n"

#define SEM_KEY		2007
#define SHM_KEY		2007
// Написать комментарии, отладить работу

union semnum {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
} sem_arg; // нужна для semctl

int main (void) {
	int shm_id, sem_id;
	char *shm_buf;
	int shm_size;
	struct shmid_ds ds;
	struct sembuf sb[1]; // массив с семафорами
	unsigned short sem_vals[1]; // массив с текущими значениями семафоров
	
	shm_id = shmget(SHM_KEY, SHMEM_SIZE, IPC_CREAT | 0600); // по одному ключу получаем один айдишник шейрд мема в 3 и 4
	if (shm_id == -1) {
		fprintf (stderr, "shmget() error\n");
		return 1;
	}
    fprintf (stderr, "[3] shm_ID: %d\n", shm_id);

	sem_id = semget(SEM_KEY, 2, 0600 | IPC_CREAT); // в 3й создаём id набора из 2х семафоров
	if (sem_id == -1) {
		fprintf (stderr, "[3] semget() error\n");
		return 1;
	}
	fprintf (stderr, "[3] semaphore_set_id: %d\n", sem_id);

	sem_vals[0] = 0;
    sem_vals[1] = 1; // зачем-то 2й семафор...
    fprintf(stderr, "[3] sem_vals 0: %d, sem_vals 1: %d\n", sem_vals[0], sem_vals[1]);

	sem_arg.array = sem_vals; // для semctl
	if (semctl(sem_id, 0, SETALL, sem_arg) == -1) { // в случае SETALL 2й параметр игнор, то есть сейчас устанавливаем семафорам в соответствии с sem_arg
		fprintf (stderr, "semctl() error\n");
		return 1;
	}

	shm_buf = (char *)shmat(shm_id, NULL, 0); // // подключаем одну разделяемую память в 3 и 4
	if (shm_buf == (char *) -1) {
		fprintf (stderr, "[3] buf by shmat() error\n");
		return 1;
	}
	shmctl(shm_id, IPC_STAT, &ds); // инфа о шейрд мемори
	shm_size = ds.shm_segsz;
    fprintf(stderr, "[3] shm_size: %d\n", shm_size); 
	if (shm_size < strlen (SH_MESSAGE)) { // чекаем впишется ли смска
		fprintf (stderr, "error: segsize=%d\n", shm_size);
		return 1;
	}
	strcpy(shm_buf, SH_MESSAGE); // пишем смску в буффер
	sb[0].sem_num = 0;
	sb[0].sem_flg = SEM_UNDO; // флаг позволяет автоматически отменить операции над семафорами в случае завершения процесса, который их выполнил.
	sb[0].sem_op = -1; // 3я программа для 0го семафора будет уменьшать значение
	semop(sem_id, sb, 1); // в блок процесс 3й программы (себя)

	semctl(sem_id, 1, IPC_RMID, sem_arg); // удаляем семафор 1
	shmdt(shm_buf); // отсоединяем
	shmctl(shm_id, IPC_RMID, NULL); // удаляем шейрд мемори

	return 0;
}