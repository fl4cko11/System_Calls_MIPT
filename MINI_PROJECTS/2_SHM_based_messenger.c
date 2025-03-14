#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define SHM_KEY_BASE 0xDEADBABE // магич число
#define SHM_MAXSIZE (1 << 20) // max 1 Mb message
#define DELAY (10 * 1000) // задержка
#define OFFSET 100 // сдвиг для буффера

int shmid; // индекс shm

void cleanup(int sig) {
    if (shmid != -1) {
        shmctl(shmid, IPC_RMID, NULL);
    }
    exit(0);
}

const char* get_terminal_name() {
    int fd = fileno(stdin);
    return ttyname(fd);
}

int main() {
    signal(SIGINT, cleanup); // signal для очистки shm
    
    printf("SHM-Chat 0.1\n");
    
    int shm_key;
    printf("Введите число для ключа ОДИНАКОВОЕ в ОБОИХ ТЕРМИНАЛАХ: ");
    scanf("%d", &shm_key);
    shm_key += SHM_KEY_BASE; // сдвиг по магич число для безопасности (может быть занята)

    // проверка на кол-во подкл-ч процессов
    int shmid_test = shmget(shm_key, 4096, IPC_CREAT | 0777);
    if (shmid_test == -1) {
        perror("shmget");
        exit(1);
    }
    struct shmid_ds buf;
    if (shmctl(shmid_test, IPC_STAT, &buf) == -1) { // проверка вызова shm
        perror("shmctl");
        exit(1);
    }
    if (buf.shm_nattch > 2) {
        printf("Only tet-a-tet chat is supported! Now exiting... (%lu)\n", (unsigned long)buf.shm_nattch);
        exit(1);
    }

    // подключаем Shm, создаём доч процесс и буффер mem
    if ((shmid = shmget(shm_key, 4096, IPC_CREAT | 0777)) == -1) {
        perror("shmget");
        exit(1);
    }
    pid_t childpid;
    if ((childpid = fork()) == -1) {
        perror("fork error");
        exit(1);
    }
    char* mem = (char*)shmat(shmid, NULL, 0);
    if (mem == (char*)-1) {
        perror("shmat");
        exit(1);
    }

    // дочерний на вывод
    if (childpid == 0) {
        mem[0] = 0; // флаг на новое сообщение
        while (1) {
            if (mem[0] != 0) {
                printf(" --> %s\n", mem + OFFSET);
                mem[0] = 0;
            }
        }
    } 
    // род на ввод
    else {
        usleep(DELAY);
        while (1) {
            printf("Введите сообщение: \n");
            char message[SHM_MAXSIZE - OFFSET]; // временный буфер для сообщения
            if (fgets(message, sizeof(message), stdin) != NULL) {
                message[strcspn(message, "\n")] = 0; // Удалить '\n'

                // Получаем имя терминала
                const char* terminal_name = get_terminal_name();
                // Форматируем сообщение с указанием терминала
                snprintf(mem + OFFSET, SHM_MAXSIZE - OFFSET, "[%s] %s", terminal_name, message);
                
                mem[0] = 1; // Устанавливаем флаг нового сообщения
            }
            usleep(DELAY);
        }
    }
}