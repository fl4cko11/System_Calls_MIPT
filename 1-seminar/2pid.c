/* Пример программы печатающей значения PPID и PID для текущего процесса */

#include <sys/types.h> /*Типы данных для сис вызовов*/
#include <unistd.h> /* Функции для сис вызовов*/
#include <stdio.h>
#include <stdlib.h>
 
void main()
{
pid_t pid, ppid;
 
pid = getpid();
ppid = getppid();
 
printf("My pid = %d, my ppid = %d\n", pid, ppid);
}