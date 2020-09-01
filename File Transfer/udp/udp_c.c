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


int main(){
  char *ip = "127.0.0.1"; //localhost
  int port = 8083;
  int e,fd;
  int l = sizeof(struct sockaddr_in);
  int sockfd;
  struct sockaddr_in server_addr;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0) {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  
//------------------------SEND-------------------------------------------------

  if ((fd = open("Maroon-5-Memories-01.mp3", O_RDONLY)) == -1)
	{
		perror("[-]Open fail\n");
		return EXIT_FAILURE;
	}
	else
  	printf("[+]File opened succesfully.\n[+]Sending...\n");
	
	long int n=0,m;
  	char buf[SIZE];

  	n = read(fd, buf, SIZE);
	while (n)
	{
		if (n == -1)
		{
			perror("[-]Read fail\n");
			return EXIT_FAILURE;
		}
		
		m = sendto(sockfd, buf, n, 0, (struct sockaddr *)&server_addr, l);
		if (m == -1)
		{
			perror("[-]Send error\n");
			return EXIT_FAILURE;
		}
		bzero(buf, SIZE);
		n = read(fd, buf, SIZE);
	}
	m = sendto(sockfd, buf, n, 0, (struct sockaddr *)&server_addr, l);

//-------------------------------------------------------------------------------
	
  	printf("[+]File data sent successfully.\n");
  	close(sockfd);

  	return 0;
}
