#include <sys/types.h>
#include <fcntl.h>   /* Все функции для работы с файлом */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int fd;
    ssize_t size;
    char buffer[4096];  // Используем больший буфер для чтения
    const char *source_file;
    const char *dest_file = "copy_of_for_5.txt";

    /* Проверка корректности ввода в командной строке */
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Открываем файл для чтения */
    source_file = argv[1];
    if ((fd = open(source_file, O_RDONLY)) < 0) {
        /* Если файл открыть не удалось, печатаем об этом сообщение и прекращаем работу */
        perror("Can't open source file");
        exit(EXIT_FAILURE);
    }

    /* Открываем файл для записи */
    int dest_fd;
    if ((dest_fd = open(dest_file, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) {
        perror("Can't open destination file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    /* Читаем исходный файл и записываем данные в новый файл */
    while ((size = read(fd, buffer, sizeof(buffer))) > 0) {
        // Записываем данные в новый файл
        if (write(dest_fd, buffer, size) != size) {
            perror("Error writing to destination file");
            close(fd);
            close(dest_fd);
            exit(EXIT_FAILURE);
        }

        // Выводим прочитанные данные в консоль с помощью STDOUT_FILENO
        if (write(STDOUT_FILENO, buffer, size) != size) {
            perror("Error writing to stdout");
            close(fd);
            close(dest_fd);
            exit(EXIT_FAILURE);
        }
    }

    if (size < 0) {
        perror("Error reading from source file");
    }

    /* Закрываем файлы */
    if (close(fd) < 0) {
        perror("Can't close source file");
    }
    if (close(dest_fd) < 0) {
        perror("Can't close destination file");
    }

    /* Открываем файл в редакторе (например, nano) */
    char command[100];
    snprintf(command, sizeof(command), "nano %s", dest_file);
    system(command);

    return 0;
}