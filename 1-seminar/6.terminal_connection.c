#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#define FILENAME "messages.txt"
#define BUFFER_SIZE 256

volatile int running = 1; //флаг на выполнение

void write_message(const char *message) {
    FILE *file = fopen(FILENAME, "a");
    if (file == NULL) {
        perror("Ошибка открытия файла для записи");
        exit(1);
    }
    fprintf(file, "%s\n", message);
    fclose(file);
}

void read_messages() {
    char buffer[BUFFER_SIZE];
    long last_position = 0;

    while (running) {
        FILE *file = fopen(FILENAME, "r");
        if (file == NULL) {
            perror("Ошибка открытия файла для чтения");
            exit(1);
        }

        // Переходим к последней прочитанной позиции
        fseek(file, last_position, SEEK_SET);

        // Читаем новые сообщения
        while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
            printf("*ДОЧЕРНИЙ*");
            printf("%s", buffer); //выводим сообщения для другого терминала
            last_position = ftell(file);  // Обновляем позицию через ftell
        }

        fclose(file);
        sleep(1); // Ждем перед следующим чтением
    }
}

void clear_file() {
    // Очищаем файл при запуске
    FILE *file = fopen(FILENAME, "w");
    if (file == NULL) {
        perror("Ошибка очистки файла");
        exit(1);
    }
    fclose(file);
}

void handle_exit(int sig) {
    running = 0; //флаг на завершение
}

int main() {
    clear_file();  // Очищаем файл перед началом работы

    // Создаем дочерний процесс для чтения сообщений
    pid_t pid = fork();

    if (pid < 0) {
        perror("Ошибка при fork");
        exit(1);
    } else if (pid == 0) {
        // Дочерний процесс
        signal(SIGINT, handle_exit);  // Обработка сигнала прерывания (из основного прилетает при exit) и while в read заканчивается
        read_messages();
        exit(0);
    }

    // Основной процесс
    signal(SIGINT, handle_exit);  // Обработка сигнала прерывания
    while (1) {
        printf("Введите сообщение (или 'exit' для выхода) *РОДИТЕЛЬСКИЙ*: \n");
        char input[BUFFER_SIZE];
        fgets(input, BUFFER_SIZE, stdin);

        // Убираем символ новой строки
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0) {
            running = 0;
            kill(pid, SIGINT); // Завершаем дочерний процесс (при отправке exit -> sigint во все signal)
            break;
        }

        write_message(input);
    }

    return 0;
}