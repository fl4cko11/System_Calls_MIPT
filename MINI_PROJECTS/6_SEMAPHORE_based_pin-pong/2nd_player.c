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
	unsigned short sem_vals[1]; // массив с текущими значениями семафоров
	
	shm_id = shmget(SHM_KEY, SHMEM_SIZE, IPC_CREAT | 0600); // по одному ключу получаем один айдишник шейрд мема в 1 и 2 игроке
	if (shm_id == -1) {
		fprintf (stderr, "shmget() error\n");
		return 1;
	}
    fprintf (stderr, "[2nd player] shm_ID: %d\n", shm_id);

	sem_id = semget(SEM_KEY, 2, 0600 | IPC_CREAT); // по одному ключу получаем один айдишник шейрд мема в 1 и 2 игроке
	if (sem_id == -1) {
		fprintf (stderr, "[2nd player] semget() error\n");
		return 1;
	}
	fprintf (stderr, "[2nd player] semaphore_set_id: %d\n", sem_id);

	sem_vals[0] = 0;
    sem_vals[1] = 0;
	sem_arg.array = sem_vals; // для semctl
	if (semctl(sem_id, 0, SETALL, sem_arg) == -1) { // в случае SETALL 2й параметр игнор, то есть сейчас устанавливаем семафорам в соответствии с sem_arg
		fprintf (stderr, "[2nd player] semctl() error\n");
		return 1;
	}

	shm_buf = (int *)shmat(shm_id, NULL, 0); // // подключаем одну разделяемую память в 3 и 4
	if (shm_buf == (int *) -1) {
		fprintf (stderr, "[2nd player] buf by shmat() error\n");
		return 1;
	}

    pid_t pid_child = fork(); //один процесс на считку, другой на работу

	if (pid_child < 0) {
		fprintf(stderr, "[2nd player] fork error\n");
		exit(-1);
	}

	else if (pid_child == 0) {
		fprintf(stderr, "[2nd player] pid of scan process: %d\n", getpid());
		shm_buf[1] = getpid();

		printf("[2nd player] enter num:\n");
		scanf("%d", &shm_buf[3]);

		shm_buf[2] = 2; // флаг, кто записал

		char command[50];
		snprintf(command, sizeof(command), "kill -9 %d", shm_buf[0]);
		int ret = system(command); //завершаем процесс скана у 1го игрока, если ввели
		if (ret == -1) {
			perror("Ошибка при выполнении команды");
			return 1;
		}
	}

	else {
		fprintf(stderr, "[2nd player] pid of play process: %d\n", getpid());

		while (shm_buf[2] == 0) {
			fprintf(stderr, "[2nd player] waiting value for play...\n");
			sleep(2);
		}

		if (shm_buf[2] == 1) {
			
		}
		//операции 2го игрока над 1м семафором в наборе
		sb[0].sem_num = 0;
		sb[0].sem_flg = SEM_UNDO; // флаг позволяет автоматически отменить операции над семафорами в случае завершения процесса, который их выполнил.
		sb[0].sem_op = 1; // 1й семафор 2го игрока ращблокирует 1го

		//операции 2го игрока над 2м семафором в наборе
		sb[1].sem_num = 1;
		sb[1].sem_flg = SEM_UNDO; // флаг позволяет автоматически отменить операции над семафорами в случае завершения процесса, который их выполнил.
		sb[1].sem_op = -1; // 2й семафор 2го игрока блокирует себя

		while (shm_buf[3] != 0) {
			if (shm_buf[2] == 1) {
				shm_buf[3] = shm_buf[3] - 1;
				printf("[2nd player] %d\n", shm_buf[3]);
				sleep(2);
				semop(sem_id, &sb[0], 1); // разблок 1го
				semop(sem_id, &sb[1], 1); // 2й себя блокирует
			}
			else if (shm_buf[2] == 2) {
				semop(sem_id, &sb[1], 1); // себя блокируем
				shm_buf[3] = shm_buf[3] - 1;
				printf("[2nd player] %d\n", shm_buf[3]);
				sleep(2);
				semop(sem_id, &sb[0], 1); // разблок 1го
			}
		}

		fprintf(stderr, "zero time! finishing...\n");
		semctl(sem_id, 1, IPC_RMID, sem_arg); // удаляем семафор 1
		shmdt(shm_buf); // отсоединяем
	}
	return 0;
}
