/* Программа будет корректно работать, только если терминалы полключаются одновременно,
потому что в любом случае при подключении нового терминала произойдёт повторная инициализация всех переменных */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define SHM_KEY_BASE 0xDEADBABE
#define BUFFER_SIZE 4096 // размер буфера

volatile int if_need_to_write_id = 0;
volatile int if_need_to_add = 0;
volatile int if_end = 1;


void write_id_handler (int sig) {
    if_need_to_write_id = 1;
}
void start_addinf_handler (int sig) {
    if_need_to_add = 1;
}
void sigint_handler (int sig) {
    if_end = 0;
}
void stop_adding_handler (int sig) {
    if_need_to_add = 0;
}

int main() {
    signal(SIGUSR1, write_id_handler);
    signal(SIGUSR2, start_addinf_handler);
    signal(SIGINT, sigint_handler);

    // в каждом терминале подключаемся к одной shm
    int shm_key;
    printf("Введите число 3 для подключения к общей памяти: \n");
    scanf("%d", &shm_key);
    shm_key += SHM_KEY_BASE; // сдвиг по магичному числу для безопасности
    int shmid = shmget(shm_key, BUFFER_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }
    char *mem = (char *)shmat(shmid, NULL, 0);
    if (mem == (char*)-1) {
        perror("shmat");
        exit(1);
    }
    
    // кладём число для увелечения
    int it = 0;
    mem[0] = 0;

    // печатем пид процесса
    pid_t pid = getpid();
    printf("pid of process that included: %d\n", pid);

    int index_self_id = 0;
    mem[1] = 1; // кладём индекс в общую память, чтобы другие записывали в следующую ячейку памяти
    while (if_end) {
        // запись ID в SHM
        if (if_need_to_write_id == 1) {
            mem[mem[1] + 1] = pid;
            index_self_id = mem[1] +1;
            mem[1]++;
            if_need_to_write_id = 0;
        }

        // Прибавление к числу 1 и вывод его в консоль
        while (if_need_to_add == 1) {
            signal(SIGHUP, stop_adding_handler);
            mem[0] ++;
            printf("from pid: %d, after added: %d\n", pid, mem[0]);
            sleep(1);
        }
        sleep(0.5);
    }
    mem[index_self_id] = 0; //изменяем id после выхода
}