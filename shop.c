#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

int shop[5];
pthread_mutex_t mutex[5];
int number;

void fin(int ret)
{
  int i;
  for(i = 0; i < 5; i++)
  {
    pthread_mutex_destroy(&mutex[i]);
  }
  exit(ret);
}

void * client()
{
  int money;
  int i;
  int num;
  pthread_mutex_lock(&mutex[0]);
  num = number;
  number++;
  srand(time(NULL) + number);
  pthread_mutex_unlock(&mutex[0]);
  money = (rand() % 2001) + 9000;
  printf("CLIENT%d START # money = %d\n", num, money);
  while (money > 0)
  {
    sleep(3);
    i = rand() % 5;
    pthread_mutex_lock(&mutex[i]);
    if (shop[i] > 0)
    {
      if (money > shop[i])
      {
        money -= shop[i];
        shop[i] = 0;
      }
      else
      {
        shop[i] -= money;
        money = 0;
      }
      printf("client%d buying from shop%d # money = %d # money in shop = %d\n", num, i + 1, money, shop[i]);
    }
    pthread_mutex_unlock(&mutex[i]);
  }
  printf("CLIENT%d FINISH\n", num);
  pthread_exit(0);
}

void * loader()
{
  int i;
  srand(time(NULL) + 10);
  printf("LOADER START\n");
  while(1)
  {
    i = rand() % 5;
    pthread_mutex_lock(&mutex[i]);
    shop[i] += 500;
    printf("loader adds money in shop%d # money in shop = %d\n", i + 1, shop[i]);
    pthread_mutex_unlock(&mutex[i]);
    sleep(2);
  }
}

int main(void)
{
  pthread_t tid[4];
  int money;
  int i;
  int status;
  number = 1;
  srand(time(NULL));
  for (i = 0; i < 5; i++)
  {
    shop[i] = (rand() % 201) + 900;
    pthread_mutex_init(&mutex[i], NULL);
  }
  printf("SHOP OPEN\nshop1 = %d shop2 = %d shop3 = %d shop4 = %d shop5 = %d\n", shop[0], shop[1], shop[2], shop[3], shop[4]);
  pthread_create(&tid[0], NULL, loader, NULL);
  for (i = 1; i < 4; i++)
  {
    pthread_create(&tid[i], NULL, client, NULL);
  }
  for (i = 1; i < 4; i++)
  {
    pthread_join(tid[i], NULL);
  }
  pthread_cancel(tid[0]);
  printf("LOADER FINISH\n");
  printf("SHOP CLOSE\n");
  fin(0);
}
