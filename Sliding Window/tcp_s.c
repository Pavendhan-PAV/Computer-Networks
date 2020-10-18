#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int s_socket, s_server;

int ServerCreate(int port) // Return 1 for error
{
	s_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server, other;
	memset(&server, 0, sizeof(server));
	memset(&other, 0, sizeof(other));

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

	if (bind(s_socket, (struct sockaddr *)&server, sizeof(server)) != -1)
	{
		printf("\n[+]Server Running.....\n");
		listen(s_socket, 5);

		return 0;

		//int accepterror = AcceptNewClient();
		//return accepterror;
	}
	else
	{
		printf("\n[-]Error in Bind.\n");
		return 1;
	}
}

int AcceptNewClient()
{
	struct sockaddr_in other;
	memset(&other, 0, sizeof(other));
	socklen_t add = sizeof(other);

	s_server = accept(s_socket, (struct sockaddr *)&other, &add);
	if (s_server == -1)
		return 1;
	else
	{
		printf("[+]Conection accepted from %s,%d\n", inet_ntoa(other.sin_addr), ntohs(other.sin_port));
		return 0;
	}
}

int main()
{
	printf("\n----------------------Sliding Window (Selective repeat) Receiver-----------------------------\n");

	int ex = 0;

	while (ex == 0)
	{
		int port;
		printf("\nEnter Port: ");
		scanf("%d", &port);

		int window_size = 1;
		printf("Enter Window Size: ");
		scanf("%d", &window_size);

		int accepterror = 0;
		int error = ServerCreate(port);
		accepterror = AcceptNewClient();
		if (error == 1 || accepterror != 0)
		{
			close(s_server);
			close(s_socket);
			printf("[-]Server Issue.\n");
			return 0;
		}
		else
		{
			printf("[+]Server Waiting...\n");

			char data[100];
			int index = 0;

			char ack[2];
			ack[0] = '1';
			ack[1] = '\0';

			char ack_temp[2];
			int flag = 1;
			char buf[2];

			char exit[2];
			int exloop = 0;

			while (exloop == 0)
			{
				recv(s_server, exit, sizeof(exit), 0);
				if (exit[0] == '1')
				{
					flag = 0;
					exloop = 1;
					//continue;
				}
				else
				{
					printf("\n");
					for (int i = 0; i < window_size; i++)
					{
						recv(s_server, buf, sizeof(buf), 0);
						printf("Received: %s\n", buf);
						data[index] = buf[0];
						index++;
					}
					printf("\n");
					for (int i = 0; i < window_size; i++)
					{
						printf("ACK (success-1 / fail-0): ");
						scanf("%s", ack_temp);
						ack[0] = ack_temp[0];

						send(s_server, ack, sizeof(ack), 0);
						if (ack[0] == '0')
						{
							index = index - window_size + i;
							i = window_size;
						}
					}
				}
			}
			printf("\nData: %s\n", data);
		}

		close(s_server);
		close(s_socket);

		printf("\n[*]Do you want to exit (Yes-1 / No-0): ");
		scanf("%d", &ex);
	}

	printf("\n---------------------------------------------------------------------\n");
	return 0;
}