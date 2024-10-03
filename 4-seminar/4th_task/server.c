#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#define FIFO_NAME "aaa.fifo"

int main() {
    int fd;
    char buffer[256];

    // Открываем FIFO для чтения
    fd = open(FIFO_NAME, O_RDONLY);
    if (fd == -1) {
        perror("Ошибка открытия FIFO для чтения");
        exit(EXIT_FAILURE);
    }
    else {
        printf("server connected\n");
    }

    while (1) {
        // Читаем число из FIFO
        ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0'; // Завершаем строку
            char *endptr;
            long num = strtol(buffer, &endptr, 10);

            // Проверяем, является ли введенное значение числом
            if (endptr == buffer || *endptr != '\0') {
                printf("Неверный ввод. Завершение работы сервера.\n");
                break;
            }

            long square = num * num;
            printf("Квадрат числа %ld: %ld\n", num, square);
        }
    }

    close(fd);
    return 0;
}