#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include <sys/time.h> 
#include<netinet/in.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include <fcntl.h>

#include<sys/wait.h> 
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>

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

int interact(int c_sock,socklen_t add,struct sockaddr_in client_addr)
{
char msg[250];


			struct timeval tv;
			 tv.tv_sec = 5;
			
int count,timestamp;
float sum=0;//for choice ,to display result
//Command Display
struct timeval start,tss,tse, stop, delta,delta1;
char s[5][100];
strcpy(s[4],"what is 1+2?\nChoices:\n  a:3\n  b:2\n  c:3\n  d:4 \nYour Choice");
strcpy(s[3],"what is 3+2?\nChoices:\n  a:1\n  b:5\n  c:3\n  d:2 \nYour Choice");
strcpy(s[2],"what is 5+2?\nChoices:\n  a:1\n  b:2\n  c:7\n  d:4 \nYour Choice");
strcpy(s[1],"what is 7+2?\nChoices:\n  a:1\n  b:10\n  c:3\n  d:9 \nYour Choice");
strcpy(s[0],"what is 9+2?\nChoices:\n  a:1\n  b:2\n  c:11\n  d:4 \nYour Choice");

char ans[5];
ans[4]='a';
ans[3]='b';
ans[2]='c';
ans[1]='d';
ans[0]='c';

strcpy(msg,"Type \"yes\" to start the game");
send(c_sock,msg,sizeof(msg),0);
bzero(msg,250);
 
//recv yes
recv(c_sock,msg,sizeof(msg),0);

if(strcmp(msg,"yes")==0)
{

strcpy(msg,"ack");
send(c_sock,msg,sizeof(msg),0);
bzero(msg,250);

count=5;
sum=0;
while(count--)
{
strcpy(msg,s[count]);

send(c_sock,msg,sizeof(msg),0);
gettimeofday(&start, NULL);

recv(c_sock,msg,sizeof(msg),0);

gettimeofday(&stop, NULL);


		duration(&start, &stop, &delta);
		long int sec= delta.tv_sec;
		if(sec<0) sec*=-1;
		//printf("\n Time taken to receive: %ld \n", sec);
		sum+=sec;
		

 

if(msg[0]!=ans[count])
{
bzero(msg,250);
strcpy(msg,"Better luck next time");
send(c_sock,msg,sizeof(msg),0);
break;
}
else
{
bzero(msg,250);
strcpy(msg,"Continue");
send(c_sock,msg,sizeof(msg),0);
}
}
bzero(msg,250);
float avg_time = sum/5.0;

FILE *fp = fopen("Avgtime.txt","a");
fprintf(fp,"%f",avg_time);
fprintf(fp,"%s","\n");
fclose(fp);
fp = fopen("Port.txt","a");
fprintf(fp,"%d",ntohs(client_addr.sin_port));
fprintf(fp,"%s","\n");
fclose(fp);

printf("\nTotal Time Average: %f \n",sum/5);
/*strcpy(msg,"Win");
send(c_sock,msg,sizeof(msg),0);
*/


}
else
{
strcpy(msg,"Invalid Choice");
send(c_sock,msg,sizeof(msg),0);
exit(0);
}
}


int main()
{

int s_sock,c_sock,len;

FILE *fp = fopen("Avgtime.txt","w");
fclose(fp);
fp = fopen("Port.txt","w");
fclose(fp);
//socket file descriptor
s_sock=socket(AF_INET,SOCK_STREAM,0);

if(s_sock>0)
{

struct sockaddr_in server_addr,client_addr;

memset(&server_addr,0,sizeof(server_addr));
memset(&client_addr,0,sizeof(client_addr));

//server information
server_addr.sin_family=AF_INET;
server_addr.sin_port=htons(9024);
server_addr.sin_addr.s_addr=INADDR_ANY;

socklen_t add;
add=sizeof(client_addr);

if(bind(s_sock,(struct sockaddr*)&server_addr,sizeof(server_addr))>=0)
{printf("[+]Server Binded\n");
if((listen(s_sock,10))!=-1)
{
printf("[+]Server Listening for Connection\n");
len=sizeof(client_addr);
while(1)
{
c_sock=accept(s_sock,(struct sockaddr*)&client_addr,&len);

if(c_sock!=-1)
{

printf("[+]Connection accepted from IP: %s PORT: %d\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
	pid_t pid;
		if((pid=fork()) == 0)
		{
//close(s_sock);
interact(c_sock,add,client_addr);
// to compare time between multiple users
FILE *fp = fopen("Avgtime.txt","r");
char num[20],i=0;
float avg_time[5];
 while(fscanf(fp, "%s", num)!=EOF)
 {
	 avg_time[i] = atof(num);
	 i++;
 }  
   fclose(fp);

  int j,min=0;
  for(j=0;j<i;j++)
{
	if(avg_time[j] < avg_time[min])
		min = j;
}
fp = fopen("Port.txt","r");
int k=0,port;
while(k<=min)
{
	fscanf(fp,"%s",num);
	k++;
	port = atoi(num);
}
printf("Average time of winner in port %s : %f\n",num,avg_time[min]);
if(port == ntohs(client_addr.sin_port))
{
	char msg[250];
	strcpy(msg,"You win");
	send(c_sock,msg,sizeof(msg),0);
}
else
{
	char msg[250];
	strcpy(msg,"Better luck next time");
	send(c_sock,msg,sizeof(msg),0);
}

//kill(pid,SIGKILL);
}
if(pid==0)
break;

}//accept
else
{
printf("[-]ERROR FOUND AT ACCEPT:\n");
}
}
}//listen
else
{
printf("[-]ERROR FOUND AT LISTEN:\n");
}
}//bind
else
{
printf("[-]ERROR FOUND AT BIND:\n");
}
}
else
{

printf("[-]ERROR FOUND AT SOCKET:\n");
}



close(c_sock);
close(s_sock);
return 0;





}
