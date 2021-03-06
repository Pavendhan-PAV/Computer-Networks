#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>

int main()
{
	int s_sock;
	char buf[500],temp[500]="WELCOME TO CALCULATOR! (Type 'END' to terminate)\nChoose:\n1: ADDITION\n2: SUBTRACTION\n3: MULTIPLICATION\n4: DIVISION\n5: MODULUS";
	
	s_sock=socket(AF_INET,SOCK_DGRAM,0);
	
	struct sockaddr_in server,client;
	
	server.sin_family=AF_INET;
	server.sin_port=htons(4431);
	server.sin_addr.s_addr=INADDR_ANY;
	
	bind(s_sock,(struct sockaddr*)&server, sizeof(server));
		socklen_t c_len;
		c_len=sizeof(client);
	
	recvfrom(s_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, &c_len);
	if(strcmp(buf,"END")==0)
		return 0;
	printf("\nFROM: %s",buf);	
	
	strcpy(buf,"Enter number 1: ");
	sendto(s_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, sizeof(client));
	recvfrom(s_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, &c_len);
	int a=atoi(buf);
	
	strcpy(buf,"Enter number 2: ");
	sendto(s_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, sizeof(client));
	recvfrom(s_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, &c_len);
	int b=atoi(buf);
	
	strcpy(buf,temp);
	sendto(s_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, sizeof(client));
	
	int c;
	
	while(1)
	{	
		recvfrom(s_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, &c_len);
	
		if(strcmp(buf,"END")==0)
			break;
	
		printf("\nFROM: %s",buf);
		
		int choice= ((int)buf[0] -48);
		switch(choice)
		{
			case 1:
				strcpy(buf,"----ADDITION----");
				c= a+b;
				
				sprintf(buf,"Answer is %d",c);
				sendto(s_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, sizeof(client));
				
				break;
			
			case 2:
				strcpy(buf,"----SUBTRACTION----");
				c= a-b;
				
				sprintf(buf,"Answer is %d",c);
				sendto(s_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, sizeof(client));
				
				break;
				
			case 3:
				strcpy(buf,"----MULTIPLICATION----");
				c= a*b;
				
				sprintf(buf,"Answer is %d",c);
				sendto(s_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, sizeof(client));
				
				break;
			
			case 4:
				strcpy(buf,"----DIVISION----");
				c= a/b;
				
				sprintf(buf,"Answer is %d",c);
				sendto(s_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, sizeof(client));
				
				break;
			
			case 5:
				strcpy(buf,"----MODULUS----");
				c= a%b;
				
				sprintf(buf,"Answer is %d",c);
				sendto(s_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, sizeof(client));
				
				break;
				
			default:
				if(strcmp(buf,"END")==0)
					break;
				strcpy(buf,"No such case! Choose again.");
				sendto(s_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, sizeof(client));
				break;
		}
	}
	close(s_sock);
	return 0;
}
