#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>

int main()
{
	int c_sock;
	char buf[500];
	
	c_sock=socket(AF_INET,SOCK_DGRAM,0);
	
	struct sockaddr_in client;
	socklen_t * c_len;
	
	client.sin_family=AF_INET;
	client.sin_port=htons(4431);
	client.sin_addr.s_addr=INADDR_ANY;
	
	
	while(1)
	{
		
		printf("\n\nTO: ");
		scanf("%s",buf);
	
	
		send(c_sock, buf, sizeof(buf), 0);	
		//printf("\nClient to server message:  %s",buf);
		
		if(strcmp(buf,"END")==0)
			break;
			
		recvfrom(c_sock, buf, sizeof(buf), 0, (struct sockaddr *)&client, c_len);
		printf("FROM: %s",buf);
	}
	
	close(c_sock);
	return 0;
}
