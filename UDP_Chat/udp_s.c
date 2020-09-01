#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>

int main()
{
	int s_sock;
	char buf[50];
	
	s_sock=socket(AF_INET,SOCK_DGRAM,0);
	
	struct sockaddr_in server,client;
	
	server.sin_family=AF_INET;
	server.sin_port=htons(7978);
	server.sin_addr.s_addr=INADDR_ANY;
	
	bind(s_sock,(struct sockaddr*)&server, sizeof(server));
		socklen_t c_len;
		c_len=sizeof(client);
	
	while(1)
	{	
		recvfrom(s_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, &c_len);
	
		if(strcmp(buf,"END")==0)
			break;
	
		printf("\n\nMessage from client: %s",buf);
		
		printf("\nMessage to client: ");
		scanf("%s",buf);
		
		sendto(s_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, sizeof(client));
	}
	close(s_sock);
	return 0;
}
