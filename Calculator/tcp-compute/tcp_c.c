#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>

int main()
{
	int c_sock;
	char buf[1024];
	
	c_sock=socket(AF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in client;
	socklen_t * c_len;
	//memset(&client, 0, sizeof(client));
	
	client.sin_family=AF_INET;
	client.sin_port=htons(4435);
	client.sin_addr.s_addr=INADDR_ANY;
	
	if(connect(c_sock,(struct sockaddr*)&client,sizeof(client))==-1)
	{
		printf("\nServer busy! Aborting.");
		return 0;
	}
	
	while(1)
	{
		
		printf("\n\nTO: ");
		scanf("%s",buf);
	
	
		write(c_sock, buf, sizeof(buf));	
		//printf("\nClient to server message:  %s",buf);
		
		if(strcmp(buf,"END")==0)
			break;
			
		read(c_sock, buf, sizeof(buf));
		printf("FROM: %s",buf);
	}
	
	close(c_sock);
	return 0;
}
