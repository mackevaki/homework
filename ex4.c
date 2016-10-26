#include <sys/types.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/ipc.h>
#include <sys/sem.h>

int main() { 
  int fd[2], result, i;
  int answer[1];  
  int buff[2]; 

  int semid; 
  const char pathname[] = "keyc.c"; 
  key_t key; 
  
  struct sembuf mybuf;
  
  if((key = ftok(pathname, 0)) < 0) {
    printf("Can\'t generate key\n");
    exit(EXIT_FAILURE);
  }

  if(pipe(fd) < 0){
    fprintf(stderr, "Can\'t create pipe\n");
        exit(EXIT_FAILURE); 
    } 

  if ((semid = semget(key, 2 , 0666 | IPC_CREAT)) < 0) {
     fprintf(stderr, "Can`t get semid\n");
     exit(EXIT_FAILURE);
  }
 
  mybuf.sem_num = 1;
  mybuf.sem_op = 1;
  mybuf.sem_flg = 0;  

  if (semop(semid , &mybuf , 1) < 0) {
    fprintf(stderr, "Can`t wait for condition\n");
    exit(EXIT_FAILURE);
  }

  result = fork(); 
  if(result < 0){ 
    fprintf(stderr, "Can\'t fork child\n"); 
    exit(EXIT_FAILURE); 
  } 
  else if (result > 0) { /* In parent process */
    while(1){
      for(i = 0; i < 2; i++){
        buff[i] = rand() % 10;
      }
      
      write(fd[1], buff, 10);
      printf("Parent is writting a = %d, b = %d\n", buff[0], buff[1]); 

      mybuf.sem_op = -1;
      mybuf.sem_flg = 0;
      mybuf.sem_num = 0;

      if (semop(semid , &mybuf , 1) < 0) {
        fprintf(stderr, "Can`t wait for condition\n");
        exit(EXIT_FAILURE);
      }        
      sleep(3);
      /* Getting the answer */
      read(fd[0], answer, 10);      
      printf("%d\n", answer[0]); 

      mybuf.sem_op = 1;
      mybuf.sem_flg = 0;
      mybuf.sem_num = 1;

      if (semop(semid , &mybuf , 1) < 0) {
        fprintf(stderr, "Can`t wait for condition\n");
        exit(EXIT_FAILURE);
      }
      
      printf("Parent exit\n");
    } 
  } 
  else { 
    while(1){
      mybuf.sem_op = -1;
      mybuf.sem_flg = 0;  
      mybuf.sem_num = 1;

      if (semop(semid , &mybuf , 1) < 0) {
        fprintf(stderr, "Can`t wait for condition\n");
        exit(EXIT_FAILURE);
      }
      sleep(1);

      read(fd[0], buff, 10); 
      printf("Child is reading a, b\n"); 

      answer[0] = buff[0] + buff[1];
      write(fd[1], answer, 4);

      mybuf.sem_op = 1;
      mybuf.sem_flg = 0;
      mybuf.sem_num = 0;

      if (semop(semid , &mybuf , 1) < 0) {
        fprintf(stderr, "Can`t wait for condition\n");
        exit(EXIT_FAILURE);
      }
    } 
 }
 
return 0; 
}
















