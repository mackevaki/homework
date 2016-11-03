#include <sys/types.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <sys/wait.h>

#define TABLE_LIMIT N
#define NUM_OF_TYPES 6

struct sembuf mybuf;
  
typedef struct {
  char name[20];
 	int time; 
} time_op;	  
  
  
key_t get_key(const char* pathname) {
  key_t key;
  if((key = ftok(pathname, 0)) < 0) {
    printf("Can\'t generate key\n");
    exit(EXIT_FAILURE);
  }
return key;
}

int create_sem(key_t key, int n) {
  int semid;
  
  if ((semid = semget(key, n, 0666 | IPC_CREAT)) < 0) {
     fprintf(stderr, "Can`t get semid\n");
     exit(EXIT_FAILURE);
  }
return semid;  
} 

int sem_oper(int semid, int s_op, int s_num) {
  struct sembuf mybuf;
  
  mybuf.sem_op = s_op;
  mybuf.sem_flg = 0;
  mybuf.sem_num = s_num;

  if (semop(semid , &mybuf , 1) < 0) {
    fprintf(stderr, "Can`t wait for condition\n");
    exit(EXIT_FAILURE);
  }    
}

int main(int argc, char** argv) { 
  int fd[2], result, i = 0, res;
  char d_name[22]; 
  char buff[22]; 
  int d_num;
  FILE *myfile;
	time_op washer[NUM_OF_TYPES]; 
  time_op wiper[NUM_OF_TYPES];
   
  int semid; 
  const char pathname[] = "key.c"; 
  key_t key; 
  
  // Check for correct number of arguments
  // There can be only one argument - table limit
  if(argc > 3 || argc < 2) {
    fprintf(stderr, "Usage: %s N [table_limit]\n", argv[0]);
    exit(EXIT_FAILURE); 
  }

  int size = atoi(argv[1]);
  if(size < 1) {
    fprintf(stderr, "Incorrect matrix size\n");
    exit(EXIT_FAILURE);
  }
  
  int N = atoi(argv[1]);
  if (N < NUM_OF_TYPES) {
    fprintf(stderr, "Lack of space on the table\n");
    exit(EXIT_FAILURE); 
  }
  
  key = get_key(pathname);
  
  if(result = pipe(fd) < 0) {
    fprintf(stderr, "Can\'t create pipe\n");
    exit(EXIT_FAILURE); 
  }
  
  semid = create_sem(key, 1);
  sem_oper(semid, 2, 0); // Creating place on the table for clear dishes
  
  if ((myfile = fopen("washer.txt", "r")) == NULL) {
     fprintf(stderr, "Can't open file"); 
     exit(EXIT_FAILURE);
  }
     
 	while (fscanf(myfile, "%s %*c %d", washer[i].name, &(washer[i].time)) != EOF) {
    i++;
  }
  fclose(myfile);
  i = 0;
  /* Filling the structure */
  if ((myfile = fopen("wiper.txt", "r")) == NULL) {
        fprintf(stderr, "Can't open file");
        exit(EXIT_FAILURE);
  }   
  
	while (fscanf(myfile, "%s %*c %d", wiper[i].name, &(wiper[i].time)) != EOF) {
    i++;
  } 
  
  fclose(myfile);
  
  result = fork(); 
  if(result < 0){ 
    fprintf(stderr, "Can\'t fork child\n"); 
    exit(EXIT_FAILURE); 
  } 
    else if (result > 0) { 

      if ((myfile = fopen("dishes.txt", "r")) == NULL) {
        fprintf(stderr, "Can't open file");
        exit(EXIT_FAILURE);
      }
	    while (fscanf(myfile, "%s %*c %d", d_name, &d_num) != EOF) {
 		    printf("%s %d\n", d_name, d_num); 
        for(int j = 0; j < NUM_OF_TYPES; j++) { 
          if(!(strcmp(d_name, washer[j].name))) {
            for(int k = 0; k < d_num; k++) { // for k dishes of this type
              sleep(washer[j].time);
              sem_oper(semid, -1, 0);
              write(fd[1], washer[j].name, 21);
              printf("Washer has washed the %s for %d seconds \n", d_name, d_num);
            }
            break;
          }
		    }
	      i++;
	    }
      fclose(myfile);
   
  	int end = 0;
		write(fd[1], &end, 4);
		int zero = 0;
		wait(&zero);
   
  } 
  else { 
    while(res != 4) {
      res = read(fd[0], buff, 21); 
      for(int j = 0; j < NUM_OF_TYPES; j++) { 
        if(!(strcmp(buff, wiper[j].name))) {
          sem_oper(semid, 1, 0);
          sleep(wiper[i].time);
          
          printf("Wiper has wiped the %s for %d seconds\n", buff, wiper[j].time);
        }
      }  
    }
  } 
 
return 0; 
}  

