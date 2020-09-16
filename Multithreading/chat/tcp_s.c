#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include<sys/wait.h> 
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>

#define SIZE 512
char buf[SIZE];
int flag=0;

int main(){

  int port = 8082;
  int e,n,m;
  struct timeval start, stop, delta;

  int sockfd, new_sock,fd;
  struct sockaddr_in server_addr, new_addr;
  socklen_t addr_size;
  

  sockfd = socket(AF_INET, SOCK_STREAM, 0);	 
  if(sockfd < 0) {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = INADDR_ANY;

  e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(e < 0) {
    perror("[-]Error in bind");
    exit(1);
  }
  printf("[+]Binding successful.\n");

  if(listen(sockfd, 10) == 0){
		printf("[+]Listening....\n");
	}else{
		perror("[-]Error in listening\n");
    exit(1);
	}

  addr_size = sizeof(struct sockaddr_in);
  
  
//------------------------------RECV-------------------------------------------------
  
	while(1)
	{
		new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
		if(new_sock < 0)
		{
			printf("[-]Error in accepting %s.",inet_ntoa(new_addr.sin_addr));
			exit(1);
		}
		else
			printf("[+]Connection accepted from IP: %s PORT: %d\n",inet_ntoa(new_addr.sin_addr),ntohs(new_addr.sin_port));
	
		pid_t pid;
		if((pid=fork()) == 0)
		{
			close(sockfd);
			while(1)
			{
				recv(new_sock, buf, SIZE, 0);
				
				if(strncmp(buf,"END",3) == 0)
				{
					printf("\n[-]Disconnected from IP: %s PORT: %d\n",inet_ntoa(new_addr.sin_addr),ntohs(new_addr.sin_port));
					flag=1;
					break;
				}
				else
				{
					printf("\nFROM (%s): %s",inet_ntoa(new_addr.sin_addr),buf);
					
					printf("TO (%s): ",inet_ntoa(new_addr.sin_addr));
					fgets(buf,SIZE,stdin);
					send(new_sock, buf, SIZE, 0);
					bzero(buf, SIZE);
				}
			}
		}
		if(pid==0)
			break;
	}	

//------------------------------------------------------------------------------------
  
  printf("[-]Closing the connection.\n");
  close(sockfd);
  close(new_sock);
  return 0;
}


