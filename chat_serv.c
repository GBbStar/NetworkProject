#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUF_SIZE 100
#define MAX_CLNT 256
#define BUFSIZE 512
#define FILENAMESIZE 256	/* filename field size */
#define FileUpReq    01		/* file upload request */
#define FileDownReq  02		/* file download request */
#define FileAck      11		/* file upload acknowledge */
#define ExitReq		 12 	/* exit request */	

void * handle_clnt(void * arg);
void send_msg(char * msg, int len);
void error_handling(char * msg);
void * login(void* arg);
void* menu(void* arg);
void HandleFileUpload(int clnt_sock);
void FileDownloadProcess(int clnt_sock);

int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
	int clnt_sock, serv_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	char sel[2];
	pthread_t t_id1;
	pthread_t t_id2;
	void* thr_rest;
	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	pthread_mutex_init(&mutx, NULL);
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");


	while (1)
	{
		clnt_adr_sz = sizeof(clnt_adr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++] = clnt_sock;
		pthread_mutex_unlock(&mutx);

		pthread_create(&t_id1, NULL, login, (void*)&clnt_sock);
		pthread_detach(t_id1);
		/*if(pthread_join(t_id1, &thr_rest)!=0){
			error_handling("thread error");
			return -1;
		}*/
		

		//login((void*)&clnt_sock);
		
		//pthread_create(&t_id2, NULL, menu, (void*)&clnt_sock); 
		//menu((void*)&clnt_sock);
		//pthread_detach(t_id2);

		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
	}
	close(serv_sock);
	return 0;
}
void* menu(void* arg) {
	printf("시작");
	int clnt_sock = *((int*)arg);
	int str_len = 0, i;
	
	char msg[BUF_SIZE];
	char sel[2];

	int f, readn;

	//write(clnt_sock, "login fail", BUF_SIZE);
	while (1) {
		if ((str_len = read(clnt_sock, sel, sizeof(sel))) == 0) {
			if (strncmp(sel, "1", 1) == 0) {
				printf("1");
			}
			else if(strncmp(sel, "2", 1) == 0) {
				printf("2");
			}
			else if (strncmp(sel, "3", 1) == 0) {
				printf("3");
			}
			else if (strncmp(sel, "q", 1) == 0) {
				return NULL;
			}
		}
	}
	
		
}
void* login(void* arg) {
	int clnt_sock = *((int*)arg);
	int str_len = 0, i;
	char msgid[5];
	char msgpw[5];
	char msg[BUF_SIZE];
	char sel[3];
	pthread_t t_id;
	char* Pmsg1 = NULL;
	char* Pmsg2 = NULL;
	

	if ((str_len = read(clnt_sock, msgid, sizeof(msgid))) == 0)
		error_handling("Id error");
	

	if ((str_len = read(clnt_sock, msgpw, sizeof(msgpw))) == 0)
		error_handling("Password pw error");

	FILE* f = fopen("login.txt", "r");
	if (fopen == NULL)
		error_handling("file read error");

	
	int result_state = 0;
	char tmp1[20], tmp2[20];
	while (!feof(f)) {
		
		fscanf(f, "%s %s\n", tmp1, tmp2);
		if ((strcmp(tmp1, msgid) == 0) && (strcmp(tmp2, msgpw) == 0))
			result_state = 1;
		//printf("%d", result_state);
	}
	if ((strcmp(tmp1, msgid) == 0) && (strcmp(tmp2, msgpw) == 0))
		result_state = 1;
	//printf("%d", result_state);

	if (result_state) {
		//printf("trhead end");
		write(clnt_sock, "login success", BUF_SIZE);
		strcpy(sel, "");
		if ((str_len = read(clnt_sock, sel, sizeof(sel) - 1)) != 0)
		{
			//printf("%d %d %d\n", strncmp(sel, "1", 1), strncmp(sel, "2", 1), strncmp(sel, "3", 1));
			//printf("\n\n%s\n\n", sel);
			if (strncmp(sel, "1", 1) == 0) {
				//write(clnt_sock, "1 case", BUF_SIZE);

				while (1) {
					/*
					uint8_t msgType;
					ssize_t numBytesRcvd = recv(clnt_sock, &msgType, sizeof(msgType), MSG_WAITALL);
					if (numBytesRcvd == -1)
						error_handling("recv() error");
					else if (numBytesRcvd == 0)
						error_handling("peer connection closed");
					else if (numBytesRcvd != sizeof(msgType))
						error_handling("recv unexpected num");

					if (msgType == FileUpReq)
						HandleFileUpload(clnt_sock);

					else if (msgType == FileDownReq)
						FileDownloadProcess(clnt_sock);

					else if (msgType == ExitReq)
						exit(1);

					else
						printf("error_not define msg");
						*/
					int upOrdown = 0;
					char one_menu[3];
					if ((upOrdown = read(clnt_sock, one_menu, sizeof(one_menu) - 1)) != 0) {
						if (strncmp(one_menu, "1", 1) == 0) {
							char fileN[2];
							char fcontent[512];
							int file_name = read(clnt_sock, fileN, sizeof(fileN) - 1);

							strcat(fileN, "_up.txt");
							FILE* ft = fopen(fileN, "w");
							int file_cont = read(clnt_sock, fcontent, sizeof(fcontent) - 1);

							fputs(fcontent, ft);
							printf("파일 업로드 완료\n");
							fclose(ft);

						}
						else if (strncmp(one_menu, "2", 1) == 0) {
							char fileN[5];
							char tmp[100];
							char msg[100];
							int file_name = read(clnt_sock, fileN, sizeof(fileN) - 1);
							
							strcat(fileN, ".txt");
							FILE* ft = fopen(fileN, "r");
							
							strcpy(msg, "");
							while (!feof(ft)) {
								strcpy(tmp, "");
								fgets(tmp, sizeof(tmp) - 1, ft);
								strcat(msg, tmp);
							}

							write(clnt_sock, msg, sizeof(msg)-1);
							printf("파일 전송 성공\n");
						}

					}

				}
				close(clnt_sock);
				//close(serv_sock);

				return 0;

			}
			else if (strncmp(sel, "2", 1) == 0) {
				//printf("2enter");
				pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
				pthread_detach(t_id);
				return NULL;
			}
			else if (strncmp(sel, "3", 1) == 0) {
				//fputs("3", stdout);
				char mailmsg[100];
				int mail_str_len = read(clnt_sock, mailmsg, sizeof(mailmsg) - 1);
				if (mail_str_len == 0)
					error_handling("mail menu read fail\n\n");

				//printf("%d %d %d\n", strncmp(mailmsg, "1", 1), strncmp(mailmsg, "2", 1), strncmp(mailmsg, "3", 1));
				//printf("%s", mailmsg);
				if (strncmp(mailmsg, "1", 1) == 0) {
					char mailName[5];
					char mailtxt[10];
					strcpy(mailtxt, "");
					strcpy(mailName, "");
					char mailContent[512];


					mail_str_len = read(clnt_sock, mailName, sizeof(mailName) - 1);
					if (mail_str_len == 0)
						error_handling("mail name fail\n\n");


					mail_str_len = read(clnt_sock, mailContent, sizeof(mailContent));
					if (mail_str_len == 0)
						error_handling("mail content fail\n\n");

					strcat(mailtxt, mailName);
					strcat(mailtxt, ".txt");


					FILE* fp = fopen(mailtxt, "a");
					if (fp == NULL)
						error_handling("fopen error");
					fwrite(mailContent, 1, strlen(mailContent), fp);
					fclose(fp);
				}
				else if (strncmp(mailmsg, "2", 1) == 0) {
					char mailmsg[100];
					char mailname[10];
					strcpy(mailname, "");
					strcat(mailname, msgid);
					strcat(mailname, ".txt");
					//printf("%s", mailname);

					FILE* fp = fopen(mailname, "r");
					if (fp == NULL)
						error_handling("fopen error");
					fread(mailmsg, 1, sizeof(mailmsg) - 1, fp);

					fclose(fp);

					printf("%s\n", mailname);
					printf("%s\n", mailmsg);
					mailmsg[strlen(mailmsg)] = '\n';
					write(clnt_sock, mailmsg, sizeof(mailmsg));

				}
				else if (strncmp(sel, "3", 1) == 0) {
					//printf")
					return 0;
				}
				else {
					
				}

				return NULL;
			}
		}
		else {
			//write(clnt_sock, sel, BUF_SIZE);
			//printf("%d %d %d", strncmp(sel, "-1", 3), strncmp(sel, "1", 1), strncmp(sel, "2", 1));
			//printf("%d", str_len);
			return (void*)0;
		}

	}
	else {
		write(clnt_sock, "login fail", BUF_SIZE);

		return (void*)-1;
	}
}

void * handle_clnt(void * arg)
{
	int clnt_sock = *((int*)arg);
	int str_len = 0, i;
	char msg_h[BUF_SIZE];

	while ((str_len = read(clnt_sock, msg_h, sizeof(msg_h))) != 0)
		send_msg(msg_h, str_len);

	pthread_mutex_lock(&mutx);
	for (i = 0; i<clnt_cnt; i++)   // remove disconnected client
	{
		if (clnt_sock == clnt_socks[i])
		{
			while (i++<clnt_cnt - 1)
				clnt_socks[i] = clnt_socks[i + 1];
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);
	return NULL;
}
void send_msg(char * msg_h, int len)   // send to all
{
	int i;
	pthread_mutex_lock(&mutx);
	for (i = 0; i<clnt_cnt; i++)
		write(clnt_socks[i], msg_h, len);
	pthread_mutex_unlock(&mutx);
}
void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void HandleFileUpload(int clnt_sock) {
	char fileName[FILENAMESIZE];
	ssize_t numBytesRcvd = recv(clnt_sock, fileName, FILENAMESIZE, MSG_WAITALL);
	if (numBytesRcvd == -1)
		error_handling("recv() error");
	else if (numBytesRcvd == 0)
		error_handling("peer connection closed");
	else if (numBytesRcvd != FILENAMESIZE)
		error_handling("recv unexpected num");
	strcat(fileName, "-up");
	printf("fileName = %s\n", fileName);

	//
	uint32_t netFileSize;
	uint32_t fileSize;
	numBytesRcvd = recv(clnt_sock, &netFileSize, sizeof(netFileSize), MSG_WAITALL);
	if (numBytesRcvd == -1)
		error_handling("recv() error");
	else if (numBytesRcvd == 0)
		error_handling("peer connection closed");
	else if (numBytesRcvd != sizeof(netFileSize))
		error_handling("recv unexpected num");
	fileSize = ntohl(netFileSize);
	printf("fileSize = %u\n", fileSize);

	// 

	FILE* fp = fopen(fileName, "w");
	if (fp == NULL)
		error_handling("fopen error");

	uint8_t rcvdFileSize = 0;
	while (rcvdFileSize < fileSize) {
		char fileBuf[BUFSIZE];
		numBytesRcvd = recv(clnt_sock, fileBuf, BUFSIZE, 0);
		if (numBytesRcvd == -1)
			error_handling("recv error");
		else if (numBytesRcvd == 0)
			error_handling("peer connection closed");

		fwrite(fileBuf, sizeof(char), numBytesRcvd, fp);
		if (ferror(fp))
			error_handling("fwrite error");

		rcvdFileSize += numBytesRcvd;
	}

	fclose(fp);


	//
	uint8_t msgType = FileAck;
	ssize_t numBytesSent = send(clnt_sock, &msgType, sizeof(msgType), 0);
	if (numBytesSent == -1)
		error_handling("send() error");
	else if (numBytesSent != sizeof(msgType))
		error_handling("sent unexpected num");

}


void FileDownloadProcess(int clnt_sock) {
	char fileName[FILENAMESIZE];
	ssize_t numBytesRcvd = recv(clnt_sock, fileName, FILENAMESIZE, MSG_WAITALL);
	if (numBytesRcvd == -1)
		error_handling("recv() error");
	else if (numBytesRcvd == 0)
		error_handling("peer connection closed");
	else if (numBytesRcvd != FILENAMESIZE)
		error_handling("recv unexpected num");

	//
	struct stat sb;
	if (stat(fileName, &sb) < 0)
		error_handling("stat() error");
	uint32_t fileSize = sb.st_size;
	uint32_t netFileSize = htonl(fileSize);
	ssize_t numBytesSent = send(clnt_sock, &netFileSize, sizeof(netFileSize), 0);
	if (numBytesSent == -1)
		error_handling("send() error");
	else if (numBytesSent != sizeof(netFileSize))
		error_handling("sent unexpected num");


	//
	FILE* fp = fopen(fileName, "r");
	if (fp == NULL)
		error_handling("fopen error of server");

	while (!feof(fp)) {
		char fileBuf[BUFSIZE];
		size_t numBytesRead = fread(fileBuf, sizeof(char), BUFSIZE, fp);
		if (ferror(fp))
			error_handling("fread() error");

		numBytesSent = send(clnt_sock, fileBuf, numBytesRead, 0);
		if (numBytesSent == -1)
			error_handling("send() error");
		else if (numBytesSent != numBytesRead)
			error_handling("send unexpected num");
	}
	fclose(fp);

	//
	uint8_t msgType;
	numBytesRcvd = recv(clnt_sock, &msgType, sizeof(msgType), MSG_WAITALL);
	if (numBytesRcvd == -1)
		error_handling("recv() error");
	else if (numBytesRcvd == 0)
		error_handling("peer connection closed");
	else if (numBytesRcvd != sizeof(msgType))
		error_handling("recv unexpected num");

	if (msgType == FileAck)
		printf("%s 파일 전송 성공!!(%u Bytes)\n", fileName, fileSize);
	else
		printf("%s 파일 전송 실패!\n", fileName);
}
