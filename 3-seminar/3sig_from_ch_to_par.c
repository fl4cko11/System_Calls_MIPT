#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

/* Дочерний процесс, анализирующий год и возвращающий различные сигналы,
 * в зависимости от его високосности.
 */

void sigusr1_handle (int sig) {
    printf ("visokos\n");
    exit(0);
}

void sigusr2_handle (int sig) {
    printf ("not visokos\n");
    exit(0);
}


int main (int argc, char ** argv) {
	int year;
	if (argc < 2) 
	{
		fprintf (stderr, "child: too few arguments\n");
		return 2;
	}
	year = atoi(argv[1]); //to int
	if (year <= 0) return 2;

    pid_t chpid;
    chpid = fork();
    if (chpid == 0) {
        sleep(1); //time to reg by par
	    if ( ((year%4 == 0) && (year%100 != 0)) || (year%400 == 0) ) //если на 4 делится и не на 100 или на 400 => високостный по опр-ю
		    kill (getppid(), SIGUSR1); //sigus - нейтральный сигнал для хендлера
	    else
		    kill (getppid(), SIGUSR2);
    }
    else {
        signal(SIGUSR1, sigusr1_handle);
        signal(SIGUSR2, sigusr2_handle);
        pause(); //wait for signal
    }
	return 0;
}