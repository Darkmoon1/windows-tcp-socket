// TCP_server.cpp : 定义控制台应用程序的入口点。
//

#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

void Send(SOCKET sockClient)
{
	char sendBuff[100];
	char buff[1024];
	int byte = 0;
	char sw = 'l';

	while (true)
	{
		printf("Input what you want...\n l to send text f to send file q to exit\n");
		scanf("%s %c", sendBuff,&sw);
		getchar();
		if ('l' == sw)
		{
			byte = send(sockClient, sendBuff, strlen(sendBuff) + 1, 0);
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
				size = fread(buff, sizeof(char), sizeof(buff),fp);
				if (send(sockClient,buff,size,0)==SOCKET_ERROR)
				{
					printf("Send file failed...\n");
					break;
				}

			}
			
		}
		else if ('q'==sw)
		{
			break;
		}
		else
		{
			printf("input error exit");
		}

		
	}
	shutdown(sockClient, SD_SEND);
	closesocket(sockClient);
}

void Recv(SOCKET sockClient)
{
	char revBuff[1024];
	int byte = 0;
	while (true)
	{
		printf("Input l to recv text or f to recv file q to exit\n");
		char sw = 'l';
		scanf("%c", &sw);
		getchar();
		if ('l' == sw)
		{
			byte = recv(sockClient, revBuff, strlen(revBuff) + 1, 0);
			if (byte<0)
			{
				printf("Receive failed...\n");
				break;
			}

			printf("%s\n", revBuff);
			continue;

		}
		else if ('f' == sw)
		{
			FILE *fp = NULL;
			if ((fp = fopen("received.txt", "wb+")) == NULL)
			{
				printf("open file failed...\n");
				break;
			}
			memset(revBuff, 0, 1024);
			size_t size = 0;
			//do
			//{
			//	size = recv(sockClient, revBuff, sizeof(revBuff), MSG_WAITALL);
			//	if (size == SOCKET_ERROR)
			//	{
			//		break;
			//	}
			//	fwrite(revBuff, size, 1, fp);

			//} while (size == sizeof(revBuff));

			while ((size = recv(sockClient, revBuff, sizeof(revBuff), 0))>0)
			{
				if (fwrite(revBuff,sizeof(char),size,fp)<size)
				{
					printf("Write failed\n");
					break;
				}
				fclose(fp);
				printf("recv finished.\n");
				break;
			}
		}
		else if ('q'==sw)
		{
			break;
		}
		else
		{
			printf("input error\n");
		}
	}
	closesocket(sockClient);
}

void main(int argc, char *argv[])
{

	//initiate Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");

	//create server's socket
	SOCKET welcomeSocket;
	welcomeSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (welcomeSocket == INVALID_SOCKET)
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	//bind server socket with the port and ip address
	sockaddr_in service;

	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY;
	service.sin_port = htons(6666);

	if (bind(welcomeSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		printf("bind() failed.\n");
		closesocket(welcomeSocket);
		return;
	}
	printf("bind successsfully!\n");
	//listening socket binded with port
	if (listen(welcomeSocket, 10) == SOCKET_ERROR)
		printf("Error listening on socket.\n");
	printf("listening on socket OK.\n");
	

	while (1)
	{
		SOCKET connectionSocket = SOCKET_ERROR;
		char select;
		printf("Input s to get start...\n");
		scanf("%c", &select);
		getchar();
		if ('s'!=select)
		{
			printf("Input s to get start...\n");
			continue;
		}
		else
		{
			while(connectionSocket == SOCKET_ERROR)
			{
				printf("Waiting connection....\n");
				connectionSocket = accept(welcomeSocket, NULL, NULL);
				printf("Accept Failed Try Again...\n");
			}
			while (1)
			{
				printf("connect successfully!\nInput 1 to send file or 2 to recv file q to exit\n");
				scanf("%c", &select);
				getchar();
				if ('1' == select)
				{
					HANDLE hTread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Send, (LPVOID)connectionSocket, 0, 0);
					WaitForSingleObject(hTread1, INFINITE);
					CloseHandle(hTread1);
				}
				else if('2' == select)
				{
					HANDLE hTread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Recv, (LPVOID)connectionSocket, 0, 0);
					WaitForSingleObject(hTread2, INFINITE);
					CloseHandle(hTread2);
				}
				else if ('q' == select)
				{
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
	closesocket(welcomeSocket);
	WSACleanup();
	return;
}

