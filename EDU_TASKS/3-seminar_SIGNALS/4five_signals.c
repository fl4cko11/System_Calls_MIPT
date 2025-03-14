#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

sig_atomic_t sig_count = 0;

void sig_handler(int sig) {
    printf ("sigint!\n");
    sig_count++;
}

int main(void) {
    while (sig_count < 5) {
        signal(SIGINT, sig_handler);
        sleep(1);
    }

    printf("Получено SIGINT %d раз. Завершение...\n", sig_count);
    return 0;
}