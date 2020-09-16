#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<unistd.h>
#include <fcntl.h>
#include<poll.h>
#include<sys/wait.h> 
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>

void interact(int c_sock,socklen_t add)
{
char msg[250];
struct pollfd mypoll={STDIN_FILENO,POLLIN|POLLPRI};
//type yes
recv(c_sock,msg,sizeof(msg),0);
printf("Server: %s\n\n",msg);


scanf("%s",msg);
send(c_sock,msg,sizeof(msg),0);
printf("\n");
bzero(msg,250);


recv(c_sock,msg,sizeof(msg),0);

if(strcmp(msg,"ack")==0)
{
while(1)
{

//Question
recv(c_sock,msg,sizeof(msg),0);
printf("Server: %s\n\n",msg);

if(strcmp(msg,"You win")==0 || strcmp(msg,"Better luck next time")==0 )
break;
//printf("Client: ");
//Answer send

if( poll (&mypoll,1,60000))
{scanf("%s",msg);}
else 
{strcpy(msg,"T");
printf("Time up!!!\n");}
send(c_sock,msg,sizeof(msg),0);
printf("\n");

bzero(msg,250);
recv(c_sock,msg,sizeof(msg),0);
if(strcmp(msg,"Better luck next time")==0 )
{
printf("Server: %s\n\n",msg);
break;

}


}

}

else
{
printf("Server: %s\n",msg);
exit(0);

}
}

int main()
{

int c_sock;


//Socket Descriptor
c_sock=socket(AF_INET,SOCK_STREAM,0);
if(c_sock>0)
{

struct sockaddr_in server_addr;

//Server info
server_addr.sin_family=AF_INET;
server_addr.sin_port=htons(9024);
server_addr.sin_addr.s_addr=INADDR_ANY;

int ch=1;
socklen_t add;
add=sizeof(server_addr);
//Message Loop
if(connect(c_sock,(struct sockaddr*)&server_addr,sizeof(server_addr))>=0)
{
interact(c_sock,add);
}
else
{

printf("[-]ERROR FOUND AT CONNECTION:\n");
}

}
else
{

printf("[-]ERROR FOUND AT SOCKET:\n");
}


close(c_sock);
return 0;



}
