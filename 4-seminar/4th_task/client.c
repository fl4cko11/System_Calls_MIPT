#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FIFO_NAME "aaa.fifo"

int main() {
    int fd;
    char buffer[256];

    // Открываем FIFO для записи
    fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1) {
        perror("Ошибка открытия FIFO для записи");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Введите число (или не число для завершения): ");
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            // Удаляем символ новой строки
            buffer[strcspn(buffer, "\n")] = 0;

            // Проверяем, является ли введенное значение числом
            char *endptr;
            strtol(buffer, &endptr, 10);
            if (endptr == buffer || *endptr != '\0') {
                printf("Неверный ввод. Завершение работы клиента.\n");
                break;
            }

            // Отправляем число в FIFO
            write(fd, buffer, strlen(buffer));
        }
    }

    close(fd);
    return 0;
}