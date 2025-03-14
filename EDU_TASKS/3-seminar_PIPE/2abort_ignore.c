#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

/* Программа посылающая себе сигнал на уничтожение.*/
/* Дописать игнорирование данного сигнала.*/
void ignore_sigabrt(int sig) {
    //ignore
}

int main (void) {
    signal(SIGABRT, ignore_sigabrt); //обработчик

	pid_t dpid = getpid();
	if (kill(dpid, SIGABRT) == -1) { // посылается sigabrt в текущ процесс
		fprintf (stderr, "Cannot send signal\n");
		return 1;
	}

	printf("ignored\n");

	return 0;
}