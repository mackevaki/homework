#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h> 
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>

#define NUM_MESSAGE 1
#define N_MAX 1

char pathname[] = "x01.c"; 
int semid;

typedef struct mymsgbuf {
    long mtype;
    struct {
			int a;
			int b;
			int pid;
			int result;
		} info;
} message_t;

struct sembuf mybuf1;
void Sn_A(int n_sem, int n)
{
	mybuf1.sem_op = n;
	mybuf1.sem_flg = 0;
	mybuf1.sem_num = n_sem;
	if (semop(semid , &mybuf1 , 1) < 0) {
		printf("Can`t wait for condition\n");
		exit(EXIT_FAILURE);
	}
}

key_t get_key(char* pathname) {
  key_t key;
  if((key = ftok(pathname, 0)) < 0) {
    printf("Can\'t generate key\n");
    exit(EXIT_FAILURE);
  }
return key;
}

int open_queue(key_t key) {
  int msgid;

  if((msgid = msgget ( key, IPC_CREAT | 0660 )) < 0) {
     exit(EXIT_FAILURE);
  }
return(msgid);
}

int read_message(int msgid, long type, struct mymsgbuf *mybuf) {
   int result, len;

   len = sizeof(struct mymsgbuf) - sizeof(long);

   if((result = msgrcv( msgid, (struct msgbuf*)mybuf, len, type, 0 )) < 0) {
     exit(EXIT_FAILURE);
   }

   return(result);
}

int send_message(int msgid, struct mymsgbuf *mybuf ) {
  int result, len;
  len = sizeof(struct mymsgbuf) - sizeof(long);

  if(result = (msgsnd(msgid, (struct msgbuf*)mybuf, len, 0)) < 0)
    exit(EXIT_FAILURE);

  return(result);
}

void Sn_D(int n_sem, int n)
{
	mybuf1.sem_op = (-1)*n;
	mybuf1.sem_flg = 0;
	mybuf1.sem_num = n_sem;
	if (semop(semid , &mybuf1 , 1) < 0) {
		printf("Can`t wait for condition\n");
		exit(EXIT_FAILURE);
	}
}

void* my_thread(void* buff) {
	message_t * mybuf1 = ((message_t *) buff);
	Sn_D(0, 1);
	sleep(5);
	mybuf1->info.result = mybuf1->info.a + mybuf1->info.b;
	mybuf1->mtype = mybuf1->info.pid;	
	Sn_A(0, 1);
	
	return NULL;	
}

  
int main () {
  message_t message;
  key_t key;
  int msgid, length;
 	pthread_t thread_id;
	int result; 

  key = get_key(pathname);
  msgid = open_queue(key);

	if ((semid = semget(key , 1 , 0666 | IPC_CREAT)) < 0) 
    {
        printf("Can`t get semid\n");
        exit(EXIT_FAILURE);
	}

	Sn_A(0, N_MAX);
	
	while(1) {
		read_message(msgid, NUM_MESSAGE, &message);
		result = pthread_create(&thread_id, (pthread_attr_t *)NULL, my_thread, &message);
    send_message(msgid, &message);		
	}
  return 0;
}
