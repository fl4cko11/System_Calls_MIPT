#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// -lpthread или -pthread для успешной компиляции
void *any_func(void *arg) {
    int *a = (int *) arg; // явное преобразование у arg void* в int* и разыменовка
    *a += 2;
    return NULL; // возвращаем указатель на результат
}

int main() {
    pthread_t thread;
    int parg = 2007;

    if (pthread_create(&thread, NULL, any_func, &parg) != 0) {
        fprintf(stderr, "Error\n");
        return 1;
    }

    pthread_join(thread, NULL); // здесь free есть (NULL, тк не нужно записывать результат)
    
    // Печатаем результат
    printf("%d\n", parg);

    return 0;
}
