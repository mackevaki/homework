#include <sys/types.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define MAX_STR 100

char *text;
const char pathname[] = "key"; 
int semid;
struct sembuf mybuf;

typedef struct {
  int threadnum; // number of the thread
  int numofthread; // number of threads
} order_t; 

void sem(int n_sem, int n){
      mybuf.sem_op = n;
      mybuf.sem_flg = 0;
      mybuf.sem_num = n_sem;

      if (semop(semid , &mybuf , 1) < 0) {
        fprintf(stderr, "Can`t wait for condition\n");
        exit(EXIT_FAILURE);
      }
return NULL;
}

char* generate_text() {
  text = (char*) malloc(sizeof(char) * MAX_STR); 
  gets(text);
  printf("Получили строку: %s\n", text);
  return text;
}

char* change_sym(char *s, int i) { // i - number of symbol
  *(s + i) += i+1; 
return s;
}

void* start_routine(void* arg) { 
  sem(0,-1);
  order_t* order = (order_t*) arg;
  text = change_sym(text, order->threadnum);
  printf("Результат работы %d треда: %s\n", order->threadnum, text);
  sem(0,1);
return NULL;
}

int main(int argc, char** argv) {
  key_t key; 
  
  if((key = ftok(pathname, 0)) < 0) {
    printf("Can\'t generate key\n");
    exit(EXIT_FAILURE);
  }

  if ((semid = semget(key , 1 , 0666 | IPC_CREAT)) < 0) {
    fprintf(stderr, "Can`t get semid\n");
    exit(EXIT_FAILURE);
  }

  int numofthread;
  // Check for correct number of arguments
  // There can be only one argument - number of threads
  if(argc != 2) {
    fprintf(stderr, "Use: %s [numofthread]\n", argv[0]);  
    exit(EXIT_FAILURE);
  }

  if(argc == 2) {
    numofthread = atoi(argv[1]); 
    if(numofthread < 2) {
      fprintf(stderr, "Incorrect num of thr\n");
      exit(EXIT_FAILURE);
    }
  } 
  text = generate_text();
  printf("Data generation done!\n");
  
  pthread_t *tid = (pthread_t*) malloc(sizeof(pthread_t) * numofthread); 
  if(tid == NULL) {
    perror("Can't allocate memory for thread structures");
    exit(EXIT_FAILURE);
  }
  sem(0,1);
  for(int i = 0; i < numofthread; ++i) { 
    order_t* order = (order_t*) malloc(sizeof(order_t));
    if(order == NULL) {
      perror("Can't allocate memory for order");
      exit(EXIT_FAILURE);
    }
    
    order->threadnum = i;
    order->numofthread = numofthread;
    
    int statusresult = pthread_create(&tid[i], (pthread_attr_t *) NULL, start_routine, order);
    if(statusresult != 0) {
      perror("Creation of thread failed");
      exit(EXIT_FAILURE);
    }
  }

  for(int i = 0; i < numofthread; ++i) {
    pthread_join(tid[i], NULL);
  }

  printf("Result: %s\n", text);

  semctl(semid, IPC_RMID, 0);

return 0;
}
