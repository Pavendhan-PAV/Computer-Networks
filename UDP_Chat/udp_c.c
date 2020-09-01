#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>

int main()
{
	int c_sock;
	char buf[50];
	
	c_sock=socket(AF_INET,SOCK_DGRAM,0);
	
	struct sockaddr_in client;
	socklen_t * c_len;
	
	client.sin_family=AF_INET;
	client.sin_port=htons(7978);
	client.sin_addr.s_addr=INADDR_ANY;
	
	printf("WELCOME TO CHAT-BOX (To end the chat type 'END' as final message)");
	while(1)
	{
		
		printf("\n\nMessage to server: ");
		scanf("%s",buf);
	
	
		sendto(c_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, sizeof(client));	
		//printf("\nClient to server message:  %s",buf);
		
		if(strcmp(buf,"END")==0)
			break;
			
		recvfrom(c_sock, buf, sizeof(buf), 0, (struct sockaddr *)&client, c_len);
		printf("Message from server: %s",buf);
	}
	
	close(c_sock);
	return 0;
}
