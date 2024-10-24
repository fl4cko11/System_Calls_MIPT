#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <stdlib.h>
/* Переделать программу для доказательстава: sin*sin + cos*cos ==1
 * Квадрат синуса считать в одном потоке, косинуса во втором,
 * а результат суммировать в главной программе.*/

double *my_sin = NULL;
double *my_cos = NULL;

void *thread_func1 (void *arg) {
	fprintf (stderr, "thread1: %s\n", (char*) arg);
    my_sin = (double *)calloc(1, sizeof(double));
    *my_sin = sin(5);
	sleep (5);
	return NULL;
}

void * thread_func2 (void * arg) {
	fprintf (stderr, "thread2: %s\n", (char*) arg);
    my_cos = (double *)calloc(1, sizeof(double));
    *my_cos = cos(5);
	sleep (5);
	return NULL;
}

int main () {
	pthread_t thread1, thread2;
	char * thread1_str = "Thread1";
	char * thread2_str = "Thread2";
    void *result1;
    void *result2;
    struct timeval time_start1, time_end1;
    struct timeval time_start2, time_end2;

	if (pthread_create (&thread1, NULL, &thread_func1, thread1_str) != 0) {
		fprintf (stderr, "Error (thread1)\n");
		return 1;
	}

	if (pthread_create (&thread2, NULL, &thread_func2, thread2_str) != 0) {
		fprintf (stderr, "Error (thread2)\n");
		return 1;
	}

    sleep(2);

    pthread_cancel(thread1);
    gettimeofday(&time_start1, NULL); // Записываем стартовое время
    printf("start time sin: %ld.%06ld\n", (long)time_start1.tv_sec, (long)time_start1.tv_usec);
    pthread_cancel(thread2);
    gettimeofday(&time_start2, NULL); // Записываем стартовое время
    printf("start time cos: %ld.%06ld\n", (long)time_start2.tv_sec, (long)time_start2.tv_usec);

    // синхронизация за счёт достаточного значения sleep
    if (!pthread_equal(pthread_self(), thread1)) { // Если текущий поток не равен создаваемому
        printf("start waiting sin...\n");
        pthread_join(thread2, &result1); // Ждём завершения потока
        gettimeofday(&time_end1, NULL); // Записываем время завершения

        printf("end waiting sin: %ld.%06ld\n", (long)time_end1.tv_sec, (long)time_end1.tv_usec);
    }

    if (!pthread_equal(pthread_self(), thread2)) { // Если текущий поток не равен создаваемому
        printf("start waiting cos...\n");
        pthread_join(thread1, &result2); // Ждём завершения потока
        gettimeofday(&time_end2, NULL); // Записываем время завершения

        printf("end waiting cos: %ld.%06ld\n", (long)time_end2.tv_sec, (long)time_end2.tv_usec);
    }
    // обработка заваершений
    if (result1 == PTHREAD_CANCELED) { // Проверяем, был ли поток отменён
        long elapsed_us = (time_end1.tv_sec - time_start1.tv_sec) * 1000000 + (time_end1.tv_usec - time_start1.tv_usec);
        printf("time for end sin: %ld microseconds\n", elapsed_us);
        fprintf(stderr, "Canceled\n");
    }

    if (result2 == PTHREAD_CANCELED) { // Проверяем, был ли поток отменён
        long elapsed_us = (time_end2.tv_sec - time_start2.tv_sec) * 1000000 + (time_end2.tv_usec - time_start2.tv_usec);
        printf("time for end cos: %ld microseconds\n", elapsed_us);
        fprintf(stderr, "Canceled\n");
    }

    printf("result: %f\n", (*my_sin)*(*my_sin) + (*my_cos)*(*my_cos));

	return 0;
}