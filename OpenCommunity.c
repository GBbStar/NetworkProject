#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>
#include <windows.h>
#include <process.h> 
#include <time.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

#define RLT_SIZE 4
#define BUF_SIZE 100
#define NAME_SIZE 20
#define OPSZ 4
#define BUFSIZE 512
#define FILENAMESIZE 256	/* filename field size */
#define FileUpReq    01		/* file upload request */
#define FileDownReq  02		/* file download request */
#define FileAck      11		/* file upload acknowledge */
#define ExitReq		 12 	/* exit request */	

void HandleFileDownload(int clnt_sock, char* A_fileName);
void FileUploadProcess(int clnt_sock, char* A_fileName);
unsigned WINAPI SendMsg(void * arg);
unsigned WINAPI RecvMsg(void * arg);
unsigned WINAPI Recvlogin(void * arg);

void ErrorHandling(char * msg);

char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];

void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void clearreadbuffer(void)
{
	while (getchar() != '\n');//입력버퍼를 지우는 함
}
int main(int argc, char *argv[])
{
	WSADATA wsaData;
	SOCKET hSock;
	SOCKADDR_IN servAdr;
	HANDLE hSndThread, hRcvThread;
	char msg[BUF_SIZE];
	char sel[3];
	sel[0] = NULL;

	if (argc != 4) {
		printf("Usage : %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	}
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	sprintf(name, "[%s]", argv[3]);
	hSock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = inet_addr(argv[1]);
	servAdr.sin_port = htons(atoi(argv[2]));

	if (connect(hSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling("connect() error");

	printf("ID: ");
	scanf("%s",msg);
	//clearreadbuffer();
	//scanf("%s", msg);
	send(hSock, msg, strlen(msg), 0);
	
	strcpy(name, msg);
	//printf("%s", name);
	strcpy(msg, "");

	printf("PW: ");
	//clearreadbuffer();
	//fgets(msg, BUF_SIZE, stdin);
	
	scanf("%s", msg);
	send(hSock, msg, strlen(msg), 0);

	strcpy(msg, "");

	//printf("%s %s", name, msg);
	int strLen = recv(hSock, msg, BUF_SIZE - 1, 0);
	if (strLen == -1)
		return -1;
	msg[strLen] = 0;

	fputs(msg, stdout);
	if (strcmp(msg, "login fail") == 0) {
		fputs("나가기", stdout);
		exit(1);
	}

	char operand[50];
	char op[3];
	while (1) {
		strcpy(sel, "");
		fputs("\n\n 메뉴 목록\n", stdout);
		printf("1. 자료실 이용하기 \n");
		printf("2. 온라인 채팅 이용하기 \n");
		printf("3. 메일 이용하기 \n");
		printf("\n-1. 이용종료\n");
		scanf("%s",sel);
		send(hSock, sel, strlen(sel) - 1, 0);
		//fputs(sel, stdout);

		//printf("%d %d %d", strncmp(sel, "-1", 3), strncmp(sel, "1", 1), strncmp(sel, "2", 1));

		if (strncmp(sel, "-1", 3) == 0) {
			fputs("서비스 종료", stdout);
			closesocket(hSock);
			WSACleanup();
			exit(1);
		}
		else if (strncmp(sel, "1", 1) == 0) {

			while (1) {
				printf("\n\n\n 자료실 입장\n");
				strcpy(op, "");
				printf("---\n");
				printf("1. upload\n");
				
				printf("3. exit\n");
				//printf("q. Service Exit\n");

				printf("선택 : ");

				scanf("%s", op);
				//fputs(op, stdout);
				send(hSock, op, strlen(op) - 1, 0);

				if (strncmp(op, "1", 1) == 0) {
					//clearreadbuffer();
					//fflush(stdout);
					strcpy(operand, "");
					printf("파일이름 : ");
					//fputs("1", stdout);
					//fgets(operand, sizeof(operand) - 1, stdin);
					scanf("%s", operand);
					//fputs(operand, stdout);
					//FileUploadProcess(hSock, operand);


					FILE* ft = fopen(operand, "r");
					char tmp[100];
					char msg[512];
					send(hSock, operand, strlen(operand) - 1, 0);
					//printf("%s", operand);

					strcpy(msg, "");
					while (!feof(ft)) {
						strcpy(tmp, "");
						fgets(tmp, sizeof(tmp) - 1, ft);
						strcat(msg, tmp);
					}
					send(hSock, msg, strlen(msg) - 1, 0);
		
					fclose(ft);

				}
			
				
				else if (strncmp(op, "3", 1) == 0) {
					clearreadbuffer();
					fflush(stdout);
					strcpy(op, "");
					uint8_t msgType = ExitReq;
					size_t numBytesSent = send(hSock, &msgType, sizeof(msgType), 0);
					if (numBytesSent == -1)
						error_handling("send() error");
					else if (numBytesSent != sizeof(msgType))
						error_handling("sent unexpected num");
					break;
				}
				else
					fputs("wrong num menu\n\n", stdout);

			}

			closesocket(hSock);
			WSACleanup();
			return 0;
			//
		}
		else if (strncmp(sel, "2", 1) == 0) {
			printf("온라인 채팅방에 입장하셨습니다.\n");
			printf("이용을 그만두고 싶다면, q or Q를 입력하세요. \n");
			strcpy(msg, "");
			send(hSock, sel, strlen(sel), 0);
			//strcpy(name, "");
			hSndThread =
				(HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&hSock, 0, NULL);
			hRcvThread =
				(HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&hSock, 0, NULL);

			WaitForSingleObject(hSndThread, INFINITE);
			WaitForSingleObject(hRcvThread, INFINITE);

			closesocket(hSock);
			WSACleanup();
			return 0;

		}
		else if (strncmp(sel, "3", 1) == 0) {
			strcpy(msg, "");
			send(hSock, sel, strlen(sel), 0);
			char tempSel[3];
			char tmpName[20]; 
			char tmpContent[512];
			printf("1. 메일쓰기 \n");
			printf("2. 메일읽기 \n");
			printf("3. 메일 종료\n");
			//scanf("%d", &tempSel);
			clearreadbuffer();
			fgets(tempSel, sizeof(tempSel)-1,stdin);
			clearreadbuffer();
			//printf("%s", tempSel);
			send(hSock, tempSel, strlen(tempSel), 0);

			if (strncmp(tempSel, "1", 1) == 0) {
				
				printf("메일 상대 이름 : ");
				scanf("%s", tmpName);
				fputs("메일 내용 : ", stdout);
				clearreadbuffer();
				fgets(tmpContent, sizeof(tmpContent)-1, stdin);

				send(hSock, tmpName, strlen(tmpName), 0);
				send(hSock, tmpContent, strlen(tmpContent), 0);
				printf("%s에게 %s를 보냈습니다", tmpName, tmpContent);

			}
			else if (strncmp(tempSel, "2", 1) == 0) {
				char tempMsg[100];
				strcpy(tempMsg, " ");
				//printf("%s", msg);
				//fflush(stdout);
				printf("%s", tempMsg);

				strLen = recv(hSock, tempMsg, sizeof(tempMsg), 0);
				tempMsg[0] = ' ';
				if (strLen == -1)
					error_handling("메일 내용 읽기 실패\n");
				printf("메일 내용 : %s", tempMsg);
				
			}
			
			
			else
				printf("wrong menu select\n");

			closesocket(hSock);
			WSACleanup();
			return 0;
		}
		else if (strncmp(sel, "q", 1) == 0) {
			printf("서비스를 종료합니다\n");
			break;
		}
		else {
			closesocket(hSock);
			WSACleanup();
			return 0;
		}
	}
	/*
	strcpy(msg, "");
	strLen = recv(hSock, msg, BUF_SIZE - 1, 0);
	fputs(msg, stdout);
	*/
	
	closesocket(hSock);
	WSACleanup();
	return 0;
}

unsigned WINAPI SendMsg(void * arg)   // send thread main
{
	SOCKET hSock = *((SOCKET*)arg);
	char nameMsg[NAME_SIZE + BUF_SIZE];
	while (1)
	{
		fgets(msg, BUF_SIZE, stdin);
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
			closesocket(hSock);
			exit(0);
		}
		sprintf(nameMsg, "%s %s", name, msg);
		send(hSock, nameMsg, strlen(nameMsg), 0);
	}
	return 0;
}

unsigned WINAPI RecvMsg(void * arg)   // read thread main
{
	int hSock = *((SOCKET*)arg);
	char nameMsg[NAME_SIZE + BUF_SIZE];
	int strLen;
	while (1)
	{
		strLen = recv(hSock, nameMsg, NAME_SIZE + BUF_SIZE - 1, 0);
		if (strLen == -1)
			return -1;
		nameMsg[strLen] = 0;
		strcat(nameMsg, "	>");
		fputs(nameMsg, stdout);
	}
	return 0;
}

unsigned WINAPI Recvlogin(void * arg)   // read thread main
{	

	int hSock = *((SOCKET*)arg);
	char msg[ BUF_SIZE];
	int strLen;
	strLen = recv(hSock, msg,BUF_SIZE - 1, 0);
	if (strLen == -1) {
		printf("서버 메시지 읽기 에러");
		return -1;
	}
	msg[strLen] = 0;
	fputs(msg, stdout);
	
	return 0;
}

void ErrorHandling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}


void HandleFileDownload(int sock, char* A_fileName) {
	uint8_t msgType = FileDownReq;
	size_t numBytesSent = send(sock, &msgType, sizeof(msgType), 0);
	if (numBytesSent == -1)
		error_handling("send() error");
	else if (numBytesSent != sizeof(msgType))
		error_handling("sent unexpected num");

	//
	char fileName[FILENAMESIZE];
	memset(fileName, 0, FILENAMESIZE);
	strcpy(fileName, A_fileName);
	numBytesSent = send(sock, fileName, FILENAMESIZE, 0);
	if (numBytesSent == -1)
		error_handling("send() error");
	else if (numBytesSent != FILENAMESIZE)
		error_handling("send unexpected num");

	//
	//strcat(fileName, "_down");
	printf("fileName = %s\n", fileName);
	uint32_t netFileSize;
	uint32_t fileSize;
	size_t numBytesRcvd = recv(sock, &netFileSize, sizeof(netFileSize), MSG_WAITALL);
	if (numBytesRcvd == -1)
		error_handling("recv() error");
	else if (numBytesRcvd == 0)
		error_handling("peer connection closed");
	else if (numBytesRcvd != sizeof(netFileSize))
		error_handling("recv unexpected num");
	fileSize = ntohl(netFileSize);



	//
	
	fputs(fileName, stdout);
	FILE* fp = fopen(fileName, "w");
	if (fp == NULL)
		error_handling("fopen error of client");

	uint32_t rcvdFileSize = 0;
	while (rcvdFileSize < fileSize) {
		char fileBuf[BUFSIZE];
		numBytesRcvd = recv(sock, fileBuf, BUFSIZE, 0);
		if (numBytesRcvd == -1)
			error_handling("recv() error");
		else if (numBytesRcvd == 0)
			error_handling("peer connection closed");

		fwrite(fileBuf, sizeof(char), numBytesRcvd, fp);
		if (ferror(fp))
			error_handling("fwrite error");

		rcvdFileSize += numBytesRcvd;
	}
	printf("fileSize = %u\n 다운로드 완료 \n", fileSize);
	fclose(fp);

	//
	msgType = FileAck;
	numBytesSent = send(sock, &msgType, sizeof(msgType), 0);

	if (numBytesSent == -1)
		error_handling("send() error");
	else if (numBytesSent != sizeof(msgType))
		error_handling("sent unexptected num");
}


void FileUploadProcess(int sock, char* A_fileName) {
	uint8_t msgType = FileUpReq;
	size_t numBytesSent = send(sock, &msgType, sizeof(msgType), 0);
	if (numBytesSent == -1)
		error_handling("send() error");
	else if (numBytesSent != sizeof(msgType))
		error_handling("sent unexpected num");

	//
	char fileName[FILENAMESIZE];
	memset(fileName, 0, FILENAMESIZE);
	strcpy(fileName, A_fileName);
	numBytesSent = send(sock, fileName, FILENAMESIZE, 0);
	if (numBytesSent == -1)
		error_handling("send() error");
	else if (numBytesSent != FILENAMESIZE)
		error_handling("sent unexpected num");

	
	//printf("%s", fileName);
	//
	struct stat sb;
	if (stat(fileName, &sb) < 0)
		error_handling("stat() error");
	uint32_t fileSize = sb.st_size;
	uint32_t netFileSize = htonl(fileSize);
	numBytesSent = send(sock, &netFileSize, sizeof(netFileSize), 0);
	if (numBytesSent == -1)
		error_handling("send() error");
	else if (numBytesSent != sizeof(netFileSize))
		error_handling("sent unexpected num");


	//
	FILE* fp = fopen(fileName, "r");
	if (fp == NULL)
		error_handling("fopen error");

	while (!feof(fp)) {
		char fileBuf[BUFSIZE];
		size_t numBytesRead = fread(fileBuf, sizeof(char), BUFSIZE, fp);
		if (ferror(fp))
			error_handling("fread() error");

		numBytesSent = send(sock, fileBuf, numBytesRead, 0);
		if (numBytesSent == -1)
			error_handling("send() error");
		else if (numBytesSent != numBytesRead)
			error_handling("send unexpected num");
	}
	fclose(fp);

	//
	size_t numBytesRcvd = recv(sock, &msgType, sizeof(msgType), MSG_WAITALL);
	if (numBytesRcvd == -1)
		error_handling("recv error");
	else if (numBytesRcvd == 0)
		error_handling("peer connection closed");
	else if (numBytesRcvd != sizeof(msgType))
		error_handling("recv unexpected num");

	if (msgType == FileAck)
		printf("업로드 성공");
	else
		printf("업로드 실패");
}