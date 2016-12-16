#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_NICK_SIZE     30
#define MAX_MSG_SIZE      400
#define SERVER_PORT       53001
#define MAX_N_OF_CLIENTS  100

typedef struct {
	int IP;
	int port;
  char nickname[MAX_NICK_SIZE];
} cli_info_t;
  
cli_info_t clients[MAX_N_OF_CLIENTS], new_client;

int main() {
	int sockfd, i;
	unsigned int clilen, n, n_clients = 0;
	char line[MAX_MSG_SIZE], line1[MAX_MSG_SIZE];
  char *nickname = (char *) malloc(MAX_NICK_SIZE * sizeof(char));
	struct sockaddr_in servaddr, cliaddr, clntsaddr[MAX_N_OF_CLIENTS];
  
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  
	if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror(NULL);
		exit(EXIT_FAILURE);
	} 
	else {
		printf("Sock fd: %d\n", sockfd);
	}
  
	if (bind(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)	{
		perror(NULL);
		close(sockfd);
		exit(EXIT_FAILURE);
	}
  
	while(1) {
		bzero(line, MAX_MSG_SIZE);
		clilen = sizeof(cliaddr);
		if ((n = recvfrom(sockfd, line, MAX_MSG_SIZE - 1, 0, (struct sockaddr*)&cliaddr, &clilen)) < 0) {
			perror(NULL);
			close(sockfd);
		exit(EXIT_FAILURE);
		}
		
		new_client.IP = cliaddr.sin_addr.s_addr;
		new_client.port = cliaddr.sin_port;
		strcpy(new_client.nickname, line);
		
		int flg = 0;
		for(i = 0; i < n_clients; i++)
			if(new_client.IP == clients[i].IP && new_client.port == clients[i].port){
				strcpy(nickname, clients[i].nickname);
				flg = 1;
				break;
			}
		if(flg == 0){
			printf("В чат присоединился @%s\n", line);
			clients[n_clients].IP = new_client.IP;
			clients[n_clients].port = new_client.port;
			strcpy(clients[n_clients].nickname, line);
			clntsaddr[n_clients].sin_addr.s_addr = cliaddr.sin_addr.s_addr;
			clntsaddr[n_clients].sin_port = cliaddr.sin_port;
			n_clients++;
		}
		else{
			strcpy(line1, clients[i].nickname);
			strcat(line1, ": ");
			strcat(line1, line);
			printf("%s\n", line1);
			for(int i = 0; i < n_clients; i++){
				if(strcmp(nickname, clients[i].nickname))
					if (sendto(sockfd, line1, strlen(line1) + 1, 0, (struct sockaddr*) &clntsaddr[i], clilen) < 0) {
					  printf("%d\n",n_clients);
						perror(NULL);
						close(sockfd);
						exit(EXIT_FAILURE);
					}
			}
		}
	}
	return 0;
}
