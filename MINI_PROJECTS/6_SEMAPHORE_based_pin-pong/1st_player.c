#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define SHMEM_SIZE	4096

#define SEM_KEY		2007
#define SHM_KEY		2007

union semnum {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
} sem_arg; // нужна для semctl

int main (void) {
	int shm_id, sem_id;
	int *shm_buf;
	struct sembuf sb[1]; // массив с семафорами
	unsigned short sem_vals[1]; // массив с значениями семафоров
	
	shm_id = shmget(SHM_KEY, SHMEM_SIZE, IPC_CREAT | 0600); // по одному ключу получаем один айдишник шейрд мема в 1 и 2 игроке
	if (shm_id == -1) {
		fprintf (stderr, "shmget() error\n");
		return 1;
	}
    fprintf (stderr, "[1st player] shm_ID: %d\n", shm_id);

	sem_id = semget(SEM_KEY, 2, 0600 | IPC_CREAT); // по одному ключу получаем один айдишник набора семафоров
	if (sem_id == -1) {
		fprintf (stderr, "[1st player] semget() error\n");
		return 1;
	}
	fprintf (stderr, "[1st player] semaphore_set_id: %d\n", sem_id);
	sem_vals[0] = 0;
    sem_vals[1] = 0;
	sem_arg.array = sem_vals; // для semctl
	if (semctl(sem_id, 0, SETALL, sem_arg) == -1) { // устанавливаем семафорам в соответствии с sem_arg
		fprintf (stderr, "[1st player] semctl() error\n");
		return 1;
	}

	shm_buf = (int *)shmat(shm_id, NULL, 0); // // подключаем одну разделяемую память в 3 и 4
	if (shm_buf == (int *) -1) {
		fprintf (stderr, "[1st player] buf by shmat() error\n");
		return 1;
	}

	pid_t pid_child = fork(); //один процесс на считку, другой на работу

	if (pid_child < 0) {
		fprintf(stderr, "[1st player] fork error\n");
		exit(-1);
	}

	else if (pid_child == 0) {
		fprintf(stderr, "[1st player] pid of scan process: %d\n", getpid());
		shm_buf[0] = getpid();

		printf("[1st player] enter num:\n");
		scanf("%d", &shm_buf[3]);

		shm_buf[2] = 1; // флаг, кто записал

		char command[50];
		snprintf(command, sizeof(command), "kill -9 %d", shm_buf[1]);
		int ret = system(command); //завершаем процесс скана у 2го игрока, если ввели
		if (ret == -1) {
			perror("Ошибка при выполнении команды");
			return 1;
		}
	}

	else {
		fprintf(stderr, "[1st player] pid of play process: %d\n", getpid());

		while (shm_buf[2] == 0) {
			fprintf(stderr, "[1st player] waiting value for play...\n");
			sleep(2);
		}

		//операции 1го игрока над 1м семафором
		sb[0].sem_num = 0;
		sb[0].sem_flg = SEM_UNDO; // флаг позволяет автоматически отменить операции над семафорами в случае завершения процесса, который их выполнил.
		sb[0].sem_op = -1; // 1й семафор 1го игрока блокирует себя

		//операции 1го игрока над 2м семафором
		sb[1].sem_num = 1;
		sb[1].sem_flg = SEM_UNDO; // флаг позволяет автоматически отменить операции над семафорами в случае завершения процесса, который их выполнил.
		sb[1].sem_op = 1; // 2й семафор 2го игрока разблокирует второго игрока

		while (shm_buf[3] != 0) {
			if (shm_buf[2] == 1) {
				semop(sem_id, &sb[0], 1); // себя блокируем
				shm_buf[3] = shm_buf[3] - 1;
				printf("[1st player] %d\n", shm_buf[3]);
				sleep(2);
				semop(sem_id, &sb[1], 1); // разблок 2го
			}
			else if (shm_buf[2] == 2) {
				shm_buf[3] = shm_buf[3] - 1;
				printf("[1st player] %d\n", shm_buf[3]);
				sleep(2);
				semop(sem_id, &sb[1], 1); // разблок 2го
				semop(sem_id, &sb[0], 1); // себя блокируем
			}
		}

		fprintf(stderr, "zero time! finishing...\n");
		shmdt(shm_buf); // отсоединяем
	}
	return 0;
}