/* разобраться как работает, написать комментарии,
   в том числе ко всем параметрам. */
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char ** argv) //считываем аргументы командной
{
	int shm_id;
	char * shm_buf;
	
	if (argc < 2) //требуем больше двух аргументов ком строки 
	{
		fprintf (stderr, "Too few arguments\n");
		return 1;
	}

	shm_id = atoi(argv[1]); //id сегмента должно быть вторым в ком строке (его получаем по запуску 1й)

    //присоединяем сегмент по его ID к нашему АП
	shm_buf = (char *) shmat (shm_id, NULL, 0);
	if (shm_buf == (char *)(-1)) 
	{
		fprintf (stderr, "shmat() error\n");
		return 1;
	}

    //печатаем сообщение из 3
	printf ("Message: %s\n", shm_buf);
	shmdt (shm_buf);

	return 0;
}