#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#define SIZE 512

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int c_socket;

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct node *rptr = NULL;

struct node
{
	struct node *lcptr;
	char sym;
	int freq;
	struct node *rcptr;
};

struct lookup
{
	char s;
	char code[200];
};

char s[200];
char ds[200];
int f[200];
int n;
int nc;
struct lookup LookUpTable[200];
int lookup_size;

char encodedseq[200] = "";

////////////////////////////////////////////////////

void ISort()
{
	for (int i = 1; i < nc; i++)
	{
		for (int j = i; j > 0; j--)
		{
			if (f[j - 1] > f[j])
			{
				int temp = f[j];
				f[j] = f[j - 1];
				f[j - 1] = temp;

				char temp2 = ds[j];
				ds[j] = ds[j - 1];
				ds[j - 1] = temp2;
			}
			else
				break;
		}
	}
}

////////////////////////////////////////////////////

void Read()
{
	int fd;
	if ((fd = open("sample.txt", O_RDONLY)) == -1)
	{
		perror("\n[-]Open fail\n");
		return;
	}
	else
		printf("\n[+]File opened succesfully.\n");

	long int x = 0, m;
	char buf[SIZE];

	int start = lseek(fd, 0, SEEK_CUR);
	n = lseek(fd, 0, SEEK_END) - 1;
	lseek(fd, start, SEEK_SET);

	x = read(fd, buf, n);
	if (x == -1)
	{
		perror("[-]Read fail\n");
		return;
	}
	strcpy(s, buf);
	printf("\nString: %s", s);
	lookup_size = 0;
}

void count()
{
	nc = 0;
	for (int i = 0; i < n; i++)
	{
		int flag = 0;
		for (int j = 0; j < nc; j++)
		{
			if (s[i] == ds[j])
			{
				flag = 1;
				break;
			}
		}
		if (flag == 0)
		{
			ds[nc] = s[i];
			nc++;
		}
	}

	for (int i = 0; i < nc; i++)
	{
		int cou = 0;
		for (int j = 0; j < n; j++)
		{
			if (ds[i] == s[j])
			{
				cou++;
			}
		}
		f[i] = cou;
	}

	ISort();
}

void BuildTree(int a[], char c[], int size, struct node *ptrarr[])
{
	if (size > 1)
	{
		int b[200];
		char t[200];
		struct node *nparr[200];
		struct node *ptr = malloc(sizeof(struct node));

		int temp = a[0] + a[1];
		int tempi = 0;

		for (int i = 2; i < size; i++)
		{
			if (temp >= a[i])
			{
				tempi = i - 1;
			}
		}
		//printf("\n\ntempi - %d -- temp - %d\n\nSize - %d\n\n", tempi, temp, size);
		int ct = 2;
		for (int i = 0; i < tempi; i++)
		{
			b[i] = a[ct];
			t[i] = c[ct];
			nparr[i] = ptrarr[ct];
			ct++;
		}
		b[tempi] = temp;
		nparr[tempi] = ptr;
		t[tempi] = 0;
		for (int i = tempi + 1; i < nc - 1; i++)
		{
			b[i] = a[ct];
			t[i] = c[ct];
			nparr[i] = ptrarr[ct];
			ct++;
		}

		struct node *lfptr = malloc(sizeof(struct node));
		struct node *rgptr = malloc(sizeof(struct node));

		lfptr->sym = c[0];
		lfptr->freq = a[0];

		if (ptrarr[0] != NULL)
		{
			lfptr = ptrarr[0];
		}

		rgptr->sym = c[1];
		rgptr->freq = a[1];

		if (ptrarr[1] != NULL)
		{
			rgptr = ptrarr[1];
		}

		ptr->sym = 0;
		ptr->freq = temp;
		ptr->lcptr = lfptr;
		ptr->rcptr = rgptr;
		rptr = ptr;

		BuildTree(b, t, size - 1, nparr);
	}
}

void LookupTable(struct node *ptr, char en[], int ensize, int choice, char chars[])
{
	if (choice == 1)
	{
		if (ptr != NULL)
		{
			if (ptr->lcptr == NULL && ptr->rcptr == NULL)
			{
				lookup_size++;
				int index = (int)(ptr->sym);
				LookUpTable[index].s = ptr->sym;
				for (int i = 0; i < ensize; i++)
					LookUpTable[index].code[i] = en[i];
			}
			else
			{
				ensize++;
				en[ensize - 1] = '0';
				LookupTable(ptr->lcptr, en, ensize, 1, "");

				en[ensize - 1] = '1';
				LookupTable(ptr->rcptr, en, ensize, 1, "");
			}
		}
	}
	else if (choice == 0)
	{
		int maxnoofbits = 10;

		int noofchars = strlen(chars);

		int noofbits = 1;
		int twopow = 2;
		for (int i = 0; i < maxnoofbits; i++) ///////////////////	// MAX NO OF BITS ALLOWED = 10 // ////////////////////////
		{
			if (noofchars <= twopow)
			{
				i = maxnoofbits;
			}
			else
			{
				twopow *= 2;
				noofbits++;
			}
		}

		char code[noofbits];
		for (int i = 0; i < noofbits; i++)
			code[i] = '0';

		for (int i = 0; i < noofchars; i++)
		{
			for (int j = 0; j < noofbits; j++)
			{
				if (code[j] == '1')
					code[j] = '0';
				else if (code[j] == '0')
				{
					code[j] = '1';
					j = noofbits;
				}
			}
			LookUpTable[(int)chars[i]].s = chars[i];
			strcpy(LookUpTable[(int)chars[i]].code, code);
			printf("Code: %c - %s\n", chars[i], LookUpTable[(int)chars[i]].code);
		}
	}
}

void Encoder()
{
	printf("Encoded Sequence: ");
	char temp[400];

	for (int i = 0; i < n; i++)
	{
		int index = (int)(s[i]);

		printf("%s", LookUpTable[index].code);
		strcat(encodedseq, LookUpTable[index].code);
	}
	printf("\n");
}

void Decoder(char seq[], int nd)
{
	int cn = 0;
	printf("Decoded sequence: ");
	while (cn < nd)
	{
		struct node *ptr = rptr;
		while (ptr != NULL)
		{
			if (ptr->lcptr == NULL && ptr->rcptr == NULL)
			{
				printf("%c", ptr->sym);
				ptr = NULL;
			}
			else
			{
				if (seq[cn] == '0')
					ptr = ptr->lcptr;
				else if (seq[cn] == '1')
					ptr = ptr->rcptr;
				cn++;
			}
		}
	}
	printf("\n");
}

void print(struct node *ptr)
{
	if (ptr != NULL)
	{
		print(ptr->lcptr);
		printf("\n");
		printf("%c - %d", ptr->sym, ptr->freq);
		printf("\n");
		print(ptr->rcptr);
	}
}

int main()
{
	printf("\n----------------------Huffman Encoding Transmitter-----------------------------\n");

	rptr = NULL;

	int error = ClientCreate(9009, 1, "");
	if (error == 1)
	{
		close(c_socket);
		printf("[-]Connection Issue.\n");
		return 0;
	}
	else
	{
		int choice = 0;
		printf("\nEnter choice of Huffman Encoding(0-const, 1-var): ");
		scanf("%d", &choice);

		Read();
		count();

		printf("\nCharacters: %s \n\n", ds);
		for (int i = 0; i < nc; i++)
		{
			printf("char %c - freq %d\n", ds[i], f[i]);
		}

		if (choice == 1)
		{
			struct node *ptrarr[200];
			for (int i = 0; i < nc; i++)
				ptrarr[i] = NULL;
			BuildTree(f, ds, nc, ptrarr);
			printf("\n\n");

			printf("------------Encoder Variable Size----------\n\n");
			char en[200];
			LookupTable(rptr, en, 0, 1, "");
			Encoder();
			printf("\n-------------------------------------------\n");
		}
		else if (choice == 0)
		{
			printf("\n------------Encoder Constant Size----------\n\n");
			char en[200];
			LookupTable(rptr, en, 0, 0, ds);
			Encoder();
			printf("\n-------------------------------------------\n");
		}

		char s[1];
		char code[100];

		send(c_socket, ds, sizeof(ds), 0);

		for (int i = 0; i < strlen(ds); i++)
		{
			s[0] = LookUpTable[(int)ds[i]].s;
			strcpy(code, LookUpTable[(int)ds[i]].code);

			send(c_socket, s, sizeof(s), 0);
			send(c_socket, code, sizeof(code), 0);
		}

		send(c_socket, encodedseq, sizeof(encodedseq), 0);
		printf("\n[+]Successfully sent.\n");
	}

	close(c_socket);

	printf("\n---------------------------------------------------------------------\n");
	return 0;
}