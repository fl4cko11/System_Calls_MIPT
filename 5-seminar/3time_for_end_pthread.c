#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

void *any_func(void *arg) {
    while (1) {
        fprintf(stderr, ".\n");
        sleep(10);
    }
    return NULL;
}

int main(void) {
    pthread_t thread;
    void *result;
    struct timeval time_start, time_end;

    if (pthread_create(&thread, NULL, &any_func, NULL) != 0) {
        fprintf(stderr, "Error\n");
        return 1;
    }

    sleep(5); // Ждём 5 секунд перед отправкой сигнала на завершение

    pthread_cancel(thread); // Запрос на отмену потока
    gettimeofday(&time_start, NULL); // Записываем стартовое время

    printf("start time: %ld.%06ld\n", (long)time_start.tv_sec, (long)time_start.tv_usec);

    if (!pthread_equal(pthread_self(), thread)) { // Если текущий поток не равен создаваемому
        printf("start waiting...\n");
        pthread_join(thread, &result); // Ждём завершения потока
        gettimeofday(&time_end, NULL); // Записываем время завершения

        printf("end time: %ld.%06ld\n", (long)time_end.tv_sec, (long)time_end.tv_usec);
    }

    if (result == PTHREAD_CANCELED) { // Проверяем, был ли поток отменён
        long elapsed_us = (time_end.tv_sec - time_start.tv_sec) * 1000000 + (time_end.tv_usec - time_start.tv_usec);
        printf("time for end: %ld microseconds\n", elapsed_us);
        fprintf(stderr, "Canceled\n");
    }

    return 0;
}
