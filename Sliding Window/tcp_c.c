#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

int c_socket;

char arr[100];
int arr_size;

int ClientCreate(int port, int anyip, char IPADDR[]) // Return 1 for error
{
	c_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in client;
	memset(&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(port);
	if (anyip == 0)
		client.sin_addr.s_addr = inet_addr(IPADDR);
	else
		client.sin_addr.s_addr = INADDR_ANY;
	if (connect(c_socket, (struct sockaddr *)&client, sizeof(client)) == -1)
	{
		printf("Connection Issue.\n");
		return 1;
	}
	else
		return 0;
}

int main()
{
	printf("\n----------------------Sliding Window (Selective repeat) Transmitter-----------------------------\n");

	int exit = 0;
	char temp;

	while (exit == 0)
	{
		char ipaddr[20];
		printf("\nEnter IP Address: ");
		scanf("%s", ipaddr);

		int port;
		printf("Enter Port: ");
		scanf("%d", &port);

		int window_size = 1;
		printf("Enter Window Size: ");
		scanf("%d", &window_size);

		int error = ClientCreate(port, 0, ipaddr);
		if (error == 1)
		{
			close(c_socket);
			printf("Connection Issue.\n");
			//return 0;
		}
		else
		{
			char data[100];
			printf("\nEnter data to send: ");
			scanf("%c", &temp);
			scanf("%[^\n]", data);
			//scanf("%s", data);

			char ack[2];
			ack[0] = '1';
			ack[1] = '\0';

			char buf[2];
			buf[0] = '1';
			buf[1] = '\0';

			char exit[2];
			exit[0] = '0';
			exit[1] = '\0';

			int index = 0;

			//printf("%ld", strlen(data));
			while (index < strlen(data))
			{
				send(c_socket, exit, sizeof(exit), 0);
				printf("\n");
				for (int i = 0; i < window_size; i++)
				{
					printf("Sending: %c\n", data[index + i]);
					buf[0] = data[index + i];
					send(c_socket, buf, sizeof(buf), 0);
				}
				int ack_check = -1;
				printf("\n");
				for (int i = 0; i < window_size; i++)
				{
					recv(c_socket, ack, sizeof(ack), 0);
					printf("ACK: %s\n", ack);
					if (ack[0] == '0')
					{
						ack_check = i;
						i = window_size;
					}
				}
				if (ack_check == -1)
					index = index + window_size;
				else
					index = index + ack_check;
			}
			exit[0] = '1';
			send(c_socket, exit, sizeof(exit), 0);

			//close(c_socket);
		}

		printf("\n[*]Do you want to exit (Yes-1 / No-0): ");
		scanf("%d", &exit);

		if (exit != 0)
		{
			char exit_str[] = "/exit";
			send(c_socket, exit_str, sizeof(exit_str), 0);
			send(c_socket, exit_str, sizeof(exit_str), 0);
		}
		printf("\n");
		close(c_socket);
	}

	printf("\n---------------------------------------------------------------------\n");
	return 0;
}