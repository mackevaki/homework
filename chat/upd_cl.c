#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_MSG_SIZE 400
#define SERVER_PORT  53001

int main(int argc, char** argv)
{
	int sockfd;
	int n, result;
	char message[MAX_MSG_SIZE], recvline[MAX_MSG_SIZE];
	struct sockaddr_in servaddr, cliaddr;
  
  if(argc != 2) {
    fprintf(stderr, "Usage: a.out <IP address>\n");
    exit(EXIT_FAILURE);
  }
	if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
  
	bzero(&cliaddr, sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(0);
	cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  
	if (bind(sockfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr)) < 0){
		perror(NULL);
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
	if (inet_aton(argv[1], &servaddr.sin_addr) == 0) {
		printf("Invalid IP address\n");
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	//All created. A work with our chat:
	printf("Enter your nickname:\n");
	result = fork();
	if(result != 0) {
		while(1) {
      fflush(stdin);
			fgets(message, MAX_MSG_SIZE, stdin);
			message[strlen(message) - 1] = '\0';

			if (sendto(sockfd, message, strlen(message) + 1, 0, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        perror("Can\'t send\n");
				close(sockfd);
				exit(EXIT_FAILURE);
			}
			bzero(message, MAX_MSG_SIZE);
		}
	}
	else{
		while(1) {
			if ((n = recvfrom(sockfd, recvline, MAX_MSG_SIZE, 0, (struct sockaddr*) NULL, NULL)) < 0){
        perror("Can\'t receive\n");
				close(sockfd);
				exit(EXIT_FAILURE);
			}
			printf("%s\n", recvline);
			bzero(message, MAX_MSG_SIZE);
		}
	}
	close(sockfd);
	return 0;
}
