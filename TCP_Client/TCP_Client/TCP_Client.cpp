// TCP_Client.cpp : 定义控制台应用程序的入口点。
//

#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

void Send(SOCKET sockSever)
{
	char sendBuff[100];
	char buff[1000];
	int byte = 0;
	char sw = 'l';

	while (true)
	{
		printf("Input what you want...\n l to send text f to send file\n");
		scanf("%s %c", sendBuff, &sw);
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

		}
		else
		{
			printf("input error exit");
			break;
		}


	}
	shutdown(sockSever, SD_SEND);
	closesocket(sockSever);
}

void Recv(SOCKET sockSever)
{
	char revBuff[1000];
	int byte = 0;
	char sw = 'l';
	scanf("%c", &sw);
	while (true)
	{
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
			if ((fp = fopen("received", "wb+")) == NULL)
			{
				printf("open file failed...\n");
				break;
			}
			size_t size = 0;
			do
			{
				size = recv(sockSever, revBuff, sizeof(revBuff), MSG_WAITALL);
				if (size == SOCKET_ERROR)
				{
					break;
				}
				fwrite(revBuff, size, 1, fp);

			} while (size == sizeof(revBuff));
			fclose(fp);
			printf("recv finished.\n");
		}
	}
	closesocket(sockSever);
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

	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(ip);
	clientService.sin_port = htons(6666);

	if (connect(client, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
	{
		printf("Failed to connect.\n");
		WSACleanup();
		return;
	}

	//send and recieve data
	while (1)
	{
		char select;
		scanf("%c", &select);
		if ('s' != select)
		{
			printf("Input s to get start...\n");
			continue;
		}
		else
		{
			printf("connect successfully!\nInput 1 to send file or 2 to recv file\n");
			while (1)
			{
				scanf("%c", &select);
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
				else
				{
					printf("Error Input Try Again...\n");
					continue;
				}
			}


		}


	}
}


