#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LEN 100

void send(char *s);
void send_bit();
void get_bit(int n);

char s[MAX_LEN];
int num = 0;
pid_t pid;

int main(int argc, char **argv){
  num = 0;
  pid = getpid();

  (void)signal(SIGUSR1, get_bit);
  (void)signal(SIGUSR2, get_bit);
  (void)signal(SIGINT, send_bit);

  pid_t result = fork();

  if (result == 0) {
  } 
  else  {
    pid = result;
    gets(s);
    send_bit();
  } while(1);
    usleep(1);
    
return 0;
}
void get_bit(int n) {
  if (n == SIGUSR1)
    s[num / 8] &= ~(1 << (num % 8));
  else
    s[num / 8] |= 1 << (num % 8);
    num++;

    kill(pid, SIGINT);
    if (num == MAX_LEN * 8) {
      printf("%s\n", s);
      exit(EXIT_SUCCESS);
    }
return;
}

void send_bit() {
  char bit = s[num / 8] & (1 << num % 8);
  num++;
  usleep(1);
  bit ? kill(pid, SIGUSR1) : kill(pid, SIGUSR2);
  if (num == MAX_LEN * 8)
    exit(EXIT_SUCCESS);
return;
}

void send(char *s){
  int i, count;
  for (i = 0; i < MAX_LEN; i++)
    for (count = 0; count < 8; count++) {
      send_bit(s[i] & (1 << count));
  }
}

