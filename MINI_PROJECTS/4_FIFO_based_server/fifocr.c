#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

volatile int if_create = 0;
volatile int if_delete = 0;

void create_fifo_sigusr1_handler(int sig) {
    if_create = 1;
}

void create_fifo_sigusr2_handler(int sig) {
    if_delete = 1;
}

int main() {
    char name[]="aaa.fifo";
    printf("pid of fifo creater %d\n", getpid());

    signal(SIGUSR1, create_fifo_sigusr1_handler);
    signal(SIGUSR2, create_fifo_sigusr2_handler);

    while (1) {
        if (if_create) {
            (void)umask(0);
            mkfifo(name, 0666);
            printf("create fifo\n");
            if_create = 0;
        }

        if (if_delete) {
            if (unlink(name) == 0) {
                printf("FIFO файл '%s' успешно удалён.\n", name);
            } 
            else {
                perror("Ошибка при удалении файла");
            }
            if_delete = 0;
        }
        printf("waiting for command...\n");
        sleep(2);
    }
    return 0;
}