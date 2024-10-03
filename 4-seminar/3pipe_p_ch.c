/* Программа, осуществляющая двунаправленную связь через pipe
между процессом-родителем и процессом-ребенком */
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    int fd0[2], fd1[2], result;
    size_t size;
    char resstring[24];

    /* Создаем два pip'а */
    if(pipe(fd0) < 0) {
        printf("Can\'t create pipe\n");
        exit(-1);
    }
    if(pipe(fd1) < 0) {
    printf("Can\'t create pipe\n");
    exit(-1);
    }

    /* Порождаем новый процесс */
    result = fork();
    if(result < 0) {
        printf("Can\'t fork child\n");
        exit(-1);
    } 

    // Мы находимся в родительском процессе.
    else if (result > 0) {
        close(fd0[0]);
        close(fd1[1]); //Закрываем ненужные потоки данных

        /* Пишем в первый pipe и читаем из второго */
        size = write(fd0[1], "Hello, child!", 24);
        if(size != 24) {
            printf("Parent: can\'t write all string\n");
            exit(-1);
        }

        size = read(fd1[0], resstring, 24);
        if(size < 0) {
            printf("Parent: can\'t read string\n");
            exit(-1);
        }
        printf("Parent accept: %s\n",resstring);

        close(fd0[1]);
        close(fd1[0]);
        printf("Parent exit\n");
    }

    // Мы находимся в порожденном процессе.
    else  {
        close(fd0[1]);
        close(fd1[0]); //Закрываем ненужные потоки данных

        /* Читаем из первого pip'а и пишем во второй */
        size = read(fd0[0], resstring, 24);
        if(size < 0) {
            printf("Child: can\'t read string\n");
            exit(-1);
        }
        printf("Child accept: %s\n",resstring);

        size = write(fd1[1], "Hello, Dad!", 24);
        if(size != 24) {
            printf("Child: can\'t write all string\n");
            exit(-1);
        }

        close(fd0[0]);
        close(fd1[1]);
        printf("Child exit\n");
    }

    return 0;
} 