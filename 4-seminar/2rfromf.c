/* Программа для чтения из  FIFO */
/* Для отладки использовать утилиту strace: strace -e trace=open,read ./имя программы */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    int fd;
    size_t size;
    char resstring[25];
    char name[]="aaa.fifo";

    printf("if1\n");
    /* Открываем FIFO на чтение.*/
    if((fd = open(name, O_RDONLY)) < 0) {
        printf("Can\'t open FIFO for reading\n");
        exit(-1);
    }

    printf("if2\n");
    size = read(fd, resstring, 25);
    if(size < 0) {
        printf("Can\'t read string\n");
        exit(-1);
    }
    
    printf("if3\n");
    printf("Resstring: %s\n", resstring);

    close(fd);
    return 0;
} 