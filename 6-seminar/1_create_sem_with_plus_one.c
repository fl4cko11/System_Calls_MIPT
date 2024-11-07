#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envp[]) {
  int   semid;
  char pathname[]="1_create_sem_with_plus_one.c";
  key_t key; // для генерации ключа
  struct sembuf mybuf; // структура для одного семафора
  
  key = ftok(pathname, 0);
  
  if((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0) { // создаём семафор (получаем его айди)
    printf("Can\'t create semaphore set\n");
    exit(-1);
  }
  fprintf(stderr, "semaph_id: %d\n", semid);
  
  mybuf.sem_num = 0;
  mybuf.sem_op  = -1;
  mybuf.sem_flg = 0;

  semctl(semid, 0, SETVAL, 1); // через semctl с флагом SETVAL устанавливаем значение 1, чтобы был открыть изначально семафор (если 0 то на семопе зависнет)
  int semval = semctl(semid, 0, GETVAL);
  fprintf(stderr, "sem_val: %d\n", semval);
  
  semop(semid, &mybuf, 1);
  semval = semctl(semid, 0, GETVAL);
  fprintf(stderr, "sem_val: %d\n", semval);
  semop(semid, &mybuf, 1);

  printf("The condition is present\n");
  return 0;
}