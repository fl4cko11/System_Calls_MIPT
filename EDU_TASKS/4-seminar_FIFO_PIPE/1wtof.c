/* Программа для записи в FIFO*/
/* Для отладки использовать утилиту strace: strace -e trace=open,read ./имя программы */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

volatile int if_end = 1;

void sigusr1_handler (int sig) {
  if_end = 0;
}

int main() {
  size_t size;
  char name[]="aaa.fifo";
  printf("pid writer: %d\n", getpid());

  (void)umask(0); // обнуляем маску
  mkfifo(name, 0666); //создаём fifo, если первый вызов, то if
  printf("create fifo\n");

  // Теперь пытаемся открыть FIFO на запись
  int fd_write = open(name, O_WRONLY);
  if (fd_write < 0) {
    perror("Can't open FIFO for writing");
    close(fd_write);
    exit(-1);
  }
  printf("opened\n");

  size = write(fd_write, "Погладь Кота!", 25);
  if(size != 25) {
    printf("Can\'t write all string to FIFO\n");
    exit(-1);
  }
  printf("writed\n");

  while(if_end) {
    signal(SIGUSR1, sigusr1_handler);
    printf("writer waiting for end...\n");
    sleep(5);
  }

  close(fd_write);
  return 0;
}