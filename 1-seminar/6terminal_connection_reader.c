#include <sys/types.h>
#include <fcntl.h>   /* Все функции для работы с файлом */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE *file;
    char message[100];

    while(1){
        file = fopen("message.txt", "r");

        if (fgets(message, sizeof(message), file)){
            if


        }
    }
}