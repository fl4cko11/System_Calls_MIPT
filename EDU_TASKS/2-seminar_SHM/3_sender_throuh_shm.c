/* Проверить совместную работу с 4.
   Написать комментарии, В ТОМ ЧИСЛЕ К ПАРАМЕТРАМ!*/
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>

#define SHMEM_SIZE	4096
#define SH_MESSAGE	"Poglad Kota!\n"

int main (void)
{
	int shm_id; //для id памяти
	char * shm_buf; //буффер, которым будет указатель на присоед память
	int shm_size; //размер шэйрд мемори
	struct shmid_ds ds; //содержит характеристики шэйрд мемори
	
    //получаем ID шэйрд мемори через спец. значения
	shm_id = shmget (IPC_PRIVATE, SHMEM_SIZE, IPC_CREAT | IPC_EXCL | 0600);
	if (shm_id == -1) 
	{
		fprintf (stderr, "shmget() error\n");
		return 1;
	}

    //присоединяем сегмент по его ID к нашему АП
	shm_buf = (char *)shmat(shm_id, NULL, 0);
	if (shm_buf == (char *)(-1))
	{
		fprintf (stderr, "shmat() error\n");
		return 1;
	}

    //в структуру запишем размер сегмента, id владельца и группы, время ласт доступа и чейнжа, кол-во проц испл-х сегмент
	shmctl (shm_id, IPC_STAT, &ds);
	
    // сохраняем размер сегмента и проверяем влезает ли сообщение в сегмент
	shm_size = ds.shm_segsz;
	if (shm_size < strlen (SH_MESSAGE)) 
	{
		fprintf (stderr, "error: segsize=%d\n", shm_size);
		return 1;
	}
	
	strcpy (shm_buf, SH_MESSAGE); //переписываем заданное сообщ в буффер (т.к. здесь уже не потребуется укзаатель на сегмент)

	printf ("ID: %d\n", shm_id); //печать ID шэйрд мемори

	printf ("Press <Enter> to exit...");	
	fgetc (stdin);

	shmdt (shm_buf); //открпеляем шэйрд от нашего АП
	shmctl (shm_id, IPC_RMID, NULL); //через 2й флаг удаляем шэйрд мемори

	return 0;
}