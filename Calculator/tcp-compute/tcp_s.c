#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>

int main()
{
	int s_sock,c_sock;
	char buf[1024],temp[500]="WELCOME TO CALCULATOR! (Type 'END' to terminate)\nChoose:\n1: ADDITION\n2: SUBTRACTION\n3: MULTIPLICATION\n4: DIVISION\n5: MODULUS";
	
	s_sock=socket(AF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in server,client;
	//memset(&server, 0, sizeof(server));
	//memset(&client, 0, sizeof(client));
	
	server.sin_family=AF_INET;
	server.sin_port=htons(4435);
	server.sin_addr.s_addr=INADDR_ANY;
	
	bind(s_sock,(struct sockaddr*)&server, sizeof(server));
	socklen_t c_len;
	
	listen(s_sock,10);
	c_len=sizeof(client);
	c_sock=accept(s_sock,(struct sockaddr*)&client, &c_len);
	
	read(c_sock, buf, sizeof(buf));
	if(strcmp(buf,"END")==0)
		return 0;
	printf("\nFROM: %s",buf);	
	
	strcpy(buf,"Enter number 1: ");
	write(c_sock, buf, sizeof(buf));
	read(c_sock, buf, sizeof(buf));
	int a=atoi(buf);
	
	strcpy(buf,"Enter number 2: ");
	write(c_sock, buf, sizeof(buf));
	read(c_sock, buf, sizeof(buf));
	int b=atoi(buf);
	
	strcpy(buf,temp);
	write(c_sock, buf, sizeof(buf));
	
	int c;
	
	while(1)
	{	
		read(c_sock, buf, sizeof(buf));
	
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
				write(c_sock, buf, sizeof(buf));
				
				break;
			
			case 2:
				strcpy(buf,"----SUBTRACTION----");
				c= a-b;
				
				sprintf(buf,"Answer is %d",c);
				write(c_sock, buf, sizeof(buf));
				
				break;
				
			case 3:
				strcpy(buf,"----MULTIPLICATION----");
				c= a*b;
				
				sprintf(buf,"Answer is %d",c);
				write(c_sock, buf, sizeof(buf));
				
				break;
			
			case 4:
				strcpy(buf,"----DIVISION----");
				c= a/b;
				
				sprintf(buf,"Answer is %d",c);
				write(c_sock, buf, sizeof(buf));
				
				break;
			
			case 5:
				strcpy(buf,"----MODULUS----");
				c= a%b;
				
				sprintf(buf,"Answer is %d",c);
				write(c_sock, buf, sizeof(buf));
				
				break;
				
			default:
				if(strcmp(buf,"END")==0)
					break;
				strcpy(buf,"No such case! Choose again.");
				write(c_sock, buf, sizeof(buf));
				break;
		}
	}
	close(s_sock);
	return 0;
}
