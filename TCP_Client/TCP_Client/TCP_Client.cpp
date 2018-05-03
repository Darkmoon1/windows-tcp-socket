// TCP_Client.cpp : 定义控制台应用程序的入口点。
//

#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

void Send(SOCKET sockSever)
{
	char sendBuff[100];
	char buff[1024];
	int byte = 0;
	char sw = 'l';

	while (true)
	{
		printf("Input what you want...\n l to send text f to send file q to exit\n");
		scanf("%s %c", sendBuff, &sw);
		getchar();
		if ('l' == sw)
		{
			byte = send(sockSever, sendBuff, strlen(sendBuff) + 1, 0);
			if (byte<0)
			{
				printf("send failed.\n");
				break;
			}
		}
		else if ('f' == sw)
		{
			FILE *fp = NULL;
			if ((fp = fopen(sendBuff, "rb")) == NULL)
			{
				printf("open file failed...\n");
				break;
			}

			size_t size = 0;
			while (!feof(fp))
			{
				size = fread(buff, sizeof(char), sizeof(buff), fp);
				if (send(sockSever, buff, size, 0) == SOCKET_ERROR)
				{
					printf("Send file failed...\n");
					break;
				}

			}
			printf("Send Successfully\n");
			fclose(fp);
		}
		else if ('q'==sw)
		{
			break;
		}
		else
		{
			printf("input error \n");
		}


	}

}

void Recv(SOCKET sockSever)
{
	char revBuff[1024];
	int byte = 0;
	char sw = 'l';
	while (true)
	{
		printf("l to recv text f to recv file q to exit\n");
		scanf("%c", &sw);
		getchar();
		if ('l'==sw)
		{
			byte = recv(sockSever, revBuff, strlen(revBuff) + 1, 0);
			if (byte<0)
			{
				printf("Receive failed...\n");
				break;
			}

			printf("%s\n", revBuff);
			continue;

		}
		else if ('f'==sw)
		{
			FILE *fp = NULL;
			if ((fp = fopen("received.txt", "wb+")) == NULL)
			{
				printf("open file failed...\n");
				break;
			}
			size_t size = 0;
			memset(revBuff, 0, 1024);
			//do
			//{
			//	size = recv(socksever, revbuff, sizeof(revbuff), msg_waitall);
			//	if (size == socket_error)
			//	{
			//		break;
			//	}
			//	fwrite(revbuff, size, 1, fp);

			//} while (size == sizeof(revbuff));
			while ((size = recv(sockSever,revBuff,sizeof(revBuff),0)) >0)
			{
				if (fwrite(revBuff,sizeof(char),size,fp)<size)
				{
					printf("Write failed\n");
					break;
				}
				fclose(fp);
				printf("recv finished\n");
				break;
			}

		}
		else if ('q' == sw)
		{
			break;
		}
		else
		{
			printf("Input error");
		}
	}
}


void main(int argc, char *argv[])
{

	//initialize Winsock.
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");

	//create the client socket
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (client == INVALID_SOCKET)
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	//set up the connection
	sockaddr_in clientService;
	printf("Input serverIp..\n");
	char ip[20];
	scanf("%s", ip);
	getchar();
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(ip);
	clientService.sin_port = htons(6666);

	if (connect(client, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
	{
		printf("Failed to connect.\n");
		WSACleanup();
		system("pause");
		return;
	}

	//send and recieve data
	while (1)
	{
		char select;
		printf("Input s to get start...\n");
		scanf("%c", &select);
		getchar();
		if ('s' != select)
		{
			printf("Input s to get start...\n");
			continue;
		}
		else
		{
			while (1)
			{
				printf("connect successfully!\nInput 1 to send file or 2 to recv file q to exit\n");
				scanf("%c", &select);
				getchar();
				if ('1' == select)
				{
					HANDLE hTread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Send, (LPVOID)client, 0, 0);
					WaitForSingleObject(hTread1, INFINITE);
					CloseHandle(hTread1);
				}
				else if ('2' == select)
				{
					HANDLE hTread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Recv, (LPVOID)client, 0, 0);
					WaitForSingleObject(hTread2, INFINITE);
					CloseHandle(hTread2);
				}
				else if ('q' == select)
				{
					shutdown(client, SD_SEND);
					break;
				}
				else
				{
					printf("Error Input Try Again...\n");
					continue;
				}
			}


		}


	}
	closesocket(client);
	WSACleanup();
}


