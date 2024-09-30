#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
volatile int signals_recieved = 0; //метка на приход сигнала
const int n = 100; //порядок факториала

void sigusr1_handler (int sig) {
    signals_recieved = 1;
}

void print_time_and_result(long long unsigned int fact, struct timeval start_time) {
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    
    long seconds = current_time.tv_sec - start_time.tv_sec;

    printf("Время работы: %ld секунд, Промежуточный результат: %llu\n", 
           seconds, fact);
}

int main() {
    signal(SIGUSR1, sigusr1_handler);

    pid_t pid = getpid();
    printf("чтобы узнать факториал и время, в терминале пропишите: kill -SIGUSR1 %d\n", pid);
    printf ("pid: %d\n", pid);

    struct timeval start_time;
    gettimeofday(&start_time, NULL); // Запись времени начала

    long long unsigned int fact = 1;
    for (int i = 2; i <= n; i++) {
        fact = fact * i;
        if (signals_recieved) {
            print_time_and_result(fact, start_time);
            signals_recieved = 0;
        }
        sleep(1);
    }
}