/* добавить вторую функцию, сравнивающую количество букв,
   добавить вызов различных функций в зависимости от аргумента программы
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void check(char *a, char *b, int (*fun)(const char *, const char *)) {
  printf("Проверка на совпадение...\n");
  if(!(*fun)(a, b)) printf("Равны\n");
  else printf("Не равны\n");
}

int letters_check (const char *a, const char *b) {
    int count1 = 0;
    int count2 = 0;
    for (int i = 0; i < sizeof(a); i++){
        if ((a[i] != '\0') && (!isalpha(a[i]))) {
            count1++;
        }
    }
    for (int j = 0; j < sizeof(b); j++) {
        if ((b[j] != '\0') && (!isalpha(b[j]))) {
            count2++;
        }
    }
    if (count1 != count2){
        return 1;
    }
    else return 0;
}

int main(void)
{
  char s1[80], s2[80];
  /* указатель на функцию */
  int (*p1)(const char *, const char *);
  int (*p2)(const char *, const char *);
  //int  * p()  - без скобок будет просто функция возвращающая целое
  p1 = strcmp;
  p2 = letters_check;
  /* присваивает адрес функции strcmp указателю p */
  printf("Введите две строки.\n");
  fgets(s1, sizeof(s1), stdin);
  fgets(s2, sizeof(s2), stdin);
  check(s1, s2, p1); /* Передает адрес функции strcmp посредством указателя p */
  check (s1, s2, p2);
  return 0;
}
