#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
 
#define NUM_MESSAGE 1

typedef struct mymsgbuf {
    long mtype;
    struct {
			int a;
			int b;
			int pid;
			int result;
		} info;
} message_t;
 

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

int main(int argc, char **argv){ 

  if(argc > 4 || argc < 3) {
    fprintf(stderr, "Usage: %s a b \n", argv[0]);
    exit(EXIT_FAILURE); 
  }

  message_t message;
  int msgid;
  char pathname[] = "x01.c"; 
  key_t key; 
  int len, maxlen, k;
  
  key = get_key(pathname);
  msgid = open_queue(key);
 	int pid = getpid();
  message.mtype = NUM_MESSAGE;
  message.info.pid = pid;
  message.info.a = atoi(argv[1]);
  message.info.b = atoi(argv[2]);
  send_message(msgid, &message);
  read_message(msgid, pid, &message);

	printf("%d\n", message.info.result);
return 0;
}
 
