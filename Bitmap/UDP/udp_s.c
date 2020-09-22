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

#define SIZE 8	
off_t count=0;

int duration(struct timeval *start, struct timeval *stop, struct timeval *delta);

int main(){
  char *ip = "127.0.0.1"; //localhost
  int port = 8083;
  int e,n,m;
  struct timeval start, stop, delta;

  int sockfd,fd;
  struct sockaddr_in server_addr, new_addr;
  unsigned int l = sizeof(struct sockaddr_in);
  socklen_t addr_size;
  char buffer[SIZE];

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);	 
  if(sockfd < 0) {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(e < 0) {
    perror("[-]Error in bind");
    exit(1);
  }
  printf("[+]Binding successful.\n");

  addr_size = sizeof(struct sockaddr_in);
  
//------------------------------RECV-------------------------------------------------
  
  		gettimeofday(&start, NULL);
  		
  		if ((fd = open("new.BMP", O_CREAT | O_WRONLY | O_TRUNC, 0600)) == -1)
        {
            perror("open fail");
            exit(3);
        }
  		else
  			printf("[+]File opened succesfully.\n[+]Writing...\n");
        bzero(buffer, SIZE);
		
        n = recvfrom(sockfd, buffer, SIZE, 0, (struct sockaddr *)&new_addr, &l);
        
        while (n)
        {
            if (n == -1)
            {
                perror("[-]Recv fail");
                exit(5);
            }
            if ((m = write(fd, buffer, n)) == -1)
            {
                perror("[-]Write fail");
                exit(6);
            }
            count+=m;
            
            bzero(buffer, SIZE);
            n = recvfrom(sockfd, buffer, SIZE, 0, (struct sockaddr *)&new_addr, &l);
        }
        
        gettimeofday(&stop, NULL);
		duration(&start, &stop, &delta);
		
//------------------------------------------------------------------------------------
  close(sockfd);
  close(fd);

  printf("[+]Data written in the file successfully.\n\n");
  printf("[+]Total bytes recvd: %ld\n",count);
  printf("[+]Total duration: %ld.%ld \n", labs(delta.tv_sec), labs(delta.tv_usec));

  return 0;
}


int duration(struct timeval *start, struct timeval *stop, struct timeval *delta)
{
	suseconds_t microstart, microstop, microdelta;

	microstart = (suseconds_t)(100000 * (start->tv_sec)) + start->tv_usec;
	microstop = (suseconds_t)(100000 * (stop->tv_sec)) + stop->tv_usec;
	microdelta = microstop - microstart;

	delta->tv_usec = microdelta % 100000;
	delta->tv_sec = (time_t)(microdelta / 100000);

	if ((*delta).tv_sec < 0 || (*delta).tv_usec < 0)
		return -1;
	else
		return 0;
}


