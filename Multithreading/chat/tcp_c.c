#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>

#define SIZE 512
char buf[SIZE];

int main(){
  char *ip = "127.0.0.1"; //localhost
  int port = 8082;
  int e;
  int l = sizeof(struct sockaddr_in);
  int sockfd;
  struct sockaddr_in server_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(e == -1) {
    perror("[-]Error in socket");
    exit(1);
  }
	printf("[+]Connected to Server.\n");
  
//------------------------SEND-------------------------------------------------

	while(1)
	{
		printf("\nTO: ");
		fgets(buf,SIZE,stdin);
		send(sockfd, buf, SIZE, 0);
		
		if(strncmp(buf,"END",3) == 0)
			break;
		
		recv(sockfd, buf, SIZE, 0);
		printf("FROM: %s",buf);
	}

//-------------------------------------------------------------------------------
	
	printf("[-]Closing the connection.\n");
  	close(sockfd);

  	return 0;
}
