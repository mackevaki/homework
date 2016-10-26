#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/ipc.h>
#include <sys/sem.h>

struct sembuf mybuf;
int semid;

typedef struct {
  int k;
} order_t;

void start_routine_1(void);
void start_routine_2(void);
void* D_Sn_1(int n);
void* A_Sn_1(int n);

void* start_routine(void* arg) {
  order_t* order = (order_t*) arg;
  if (order->k == 0){
    start_routine_1();
  }
  else if (order->k == 1) 
    start_routine_2();  
  return NULL;
}

void start_routine_1(void){
  while(1){
    D_Sn_1(1);
    D_Sn_1(2);
    A_Sn_1(1); 
    A_Sn_1(2);
  }

  return;
}

void start_routine_2(void){
  while(1){
    D_Sn_1(2); 
    D_Sn_1(1);
    A_Sn_1(1);
    A_Sn_1(2);
  }
  
  return;
}

void* D_Sn_1(int n){
      mybuf.sem_op = -1;
      mybuf.sem_flg = 0;
      mybuf.sem_num = n-1;

      if (semop(semid , &mybuf , 1) < 0) {
        fprintf(stderr, "Can`t wait for condition\n");
        exit(EXIT_FAILURE);
      }
return NULL;
}

void* A_Sn_1(int n){
      mybuf.sem_op = 1;
      mybuf.sem_flg = 0;
      mybuf.sem_num = n-1;

      if (semop(semid , &mybuf , 1) < 0) {
        fprintf(stderr, "Can`t wait for condition\n");
        exit(EXIT_FAILURE);
      }
return NULL;
}

int main(int argc, char** argv) {
  const char pathname[] = "key.c"; 
  key_t key; 
  
  if((key = ftok(pathname, 0)) < 0) {
    printf("Can\'t generate key\n");
    exit(EXIT_FAILURE);
  }

  if ((semid = semget(key , 2 , 0666 | IPC_CREAT)) < 0) {
    fprintf(stderr, "Can`t get semid\n");
    exit(EXIT_FAILURE);
  }

  A_Sn_1(1); 
  A_Sn_1(2);

  pthread_t *tid = (pthread_t*) malloc(sizeof(pthread_t) * 2);
  if(tid == NULL) {
    perror("Can't allocate memory for thread structures");
    exit(EXIT_FAILURE);
  }
  
  for(int i = 0; i < 2; ++i) {
    order_t* order = (order_t*) malloc(sizeof(order_t));
    order->k = i;
    if(order == NULL) {
      perror("Can't allocate memory for order");
      exit(EXIT_FAILURE);
    }
    int startstatus = pthread_create(&tid[i], NULL, start_routine, order);
    if(startstatus != 0) {
      perror("Creation of thread failed");
      exit(EXIT_FAILURE);
    }
  }

  for(int i = 0; i < 2; ++i) {
    pthread_join(tid[i], NULL);
  }
return 0; 
}
