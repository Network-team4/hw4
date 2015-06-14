#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<pthread.h>

int BUF_SIZE = 128;
int R_BUF_SIZE = 128;
int totalbytes,totalbytes_get=0;//file size
int numTotal=0, numTotal_get=0;
void error_handling(char *message);
void read_childproc(int sig);
void* thread_put_timer(void *arg);
void* thread_get_timer(void *arg);
char fileName[129];

int main(int argc, char *argv[])
{
	//thread
	pthread_t t_id;
	int thread_param = 0;
	//
        int vRecv=0;
	int vSend = 0;
	//char fileName[129];
	int i;
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	//FILE *fp;
	pid_t pid;
	struct sigaction act;
	socklen_t adr_sz;
	int str_len, state;
	char buf[1024],buf_get[1024];
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	act.sa_handler=read_childproc;
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;
	state=sigaction(SIGCHLD, &act, 0);
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	while(1)
	{
		//int numTotal=0;
		printf("first while \n");
		int sum = 0;
		adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
		
		if(clnt_sock==-1)
			continue;
		else
			puts("new client connected...");
		pid=fork();
		if(pid==-1)
		{
			close(clnt_sock);
			continue;
		}
		else if(pid==0)//here to code
		{
			close(serv_sock);
			//FILE *fp;
			//while(1){
			//read 0 put or get
			while( (str_len = read(clnt_sock, buf, 128)) != 0)
			{
				numTotal=0;
				//int totalbytes,totalbytes_get=0;//file size
				FILE *fp;
//				char buf[129];
				printf("server while start \n");
			
				//read 0 put or get
			//	read(clnt_sock, buf, 128);
				printf("%s \n", buf);
				if( !strcmp("put", buf) ){

					//read 1 file_name
					str_len=read(clnt_sock, buf, 128);
					for(i=0;i<str_len;i++){
						fileName[i] = buf[i];
					}
					fileName[str_len] = '\0';
					printf("%s \n",fileName);
					
					//read 2 file size
					read(clnt_sock, &totalbytes, sizeof(int));
					printf("file size : %d \n", totalbytes);
					//scanf("%s", buf);
					fp = fopen(fileName,"wb");
					int cnt = 0;
					int summ = 0;

					//create thread
					if(pthread_create(&t_id, NULL, thread_put_timer, (void*)&thread_param) != 0)
					{
						puts("pthread_create() error \n");
						return -1;
					}
					//read 3 transfer file
					do{
						str_len = read(clnt_sock,buf,BUF_SIZE);
						fwrite((void*)buf, 1, str_len, fp);
						//printf("str_len : %d \n", str_len);
					        numTotal += str_len;
					        // printf("nread : %d \n",nread);
					         //printf("numtotal : %d \n",numTotal);
					}while((str_len == BUF_SIZE) && (numTotal != totalbytes));

					/*
					while( (str_len = read(clnt_sock, buf, 128)) != 0 ){
						fwrite((void*)buf, 1, str_len, fp);
						printf("str_len : %d \n", str_len);
					}
					*/	
					//shutdown(clnt_sock, SHUT_RD);		
					fclose(fp);	
					printf("finish \n");
					//scanf("%s \n", buf);
					//close(clnt_sock);
				}//put
				else if( !strcmp("get", buf) ){
					totalbytes_get = 0;
					numTotal_get=0;
					//read 1 fileName
					str_len = read(clnt_sock, buf, 128);
					for (i = 0; i < str_len; i++){
						fileName[i] = buf[i];
					}
					fileName[str_len] = '\0';
					printf("%s \n", fileName);
					
					//file I/O
					fp = fopen(fileName, "rb");
					if (fp == NULL){
						printf("no file \n");
						continue;
					}

					//file size
					fseek(fp, 0, SEEK_END);
					totalbytes_get = ftell(fp);
					printf("[%s](size:%d MB) is being received \n", fileName, totalbytes_get);

					//write 1 file_size
					write(clnt_sock, &totalbytes_get, sizeof(int));
					rewind(fp);
					//send data to client		
					//write 2 file transfer
					//create thread
					if(pthread_create(&t_id, NULL, thread_get_timer, (void*)&thread_param) != 0)
					{
						puts("pthread_create() error \n");
						return -1;
					}

					while (1){
						str_len = fread(buf_get, 1, R_BUF_SIZE, fp);
						//printf("numRead : %d \n", str_len);
						numTotal_get += str_len;
						if (str_len < R_BUF_SIZE){
							write(clnt_sock, buf_get, str_len);
							break;
						}
						usleep(1000);
						write(clnt_sock, buf_get, str_len);
					}
					//shutdown(sock, SHUT_WR);
					fclose(fp);
					printf("finish\n");
					//close(sock);
				}//get
				else if( !strcmp("q", buf) || !strcmp("Q", buf) ){
					printf("클라이언트가 종료하였습니다.\n");
					close(clnt_sock);
					close(serv_sock);
					return 0;

				}//q
				else if( !strcmp("sendrate", buf) ){
					read(clnt_sock, &vRecv, sizeof(int));
					BUF_SIZE = vRecv;
					printf("BUF_SIZE : %d \n", BUF_SIZE);
				}//sendrate
				else if( !strcmp("recvrate", buf) ){
					read(clnt_sock, &vSend, sizeof(int));
					printf("vSend : %d\n", vSend);
					R_BUF_SIZE = vSend;
					printf("R_BUF_SIZE : %d \n", R_BUF_SIZE);
					

				}
			}//while
			
			printf("client disconnected... \n");
			//puts("client disconnected...");
		}//if
		else{
			close(clnt_sock);
		}
	}//while(1)
	close(serv_sock);
	return 0;
}

void read_childproc(int sig)
{
	pid_t pid;
	int status;
	pid=waitpid(-1, &status, WNOHANG);
	printf("removed proc id: %d \n", pid);
}
void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void* thread_put_timer(void *arg)
{
	while(1)
	{
		
		printf("Transfer status : recv [%s][%d%, %d KB/ %d KB]\n", fileName, numTotal*100/totalbytes, numTotal, totalbytes);
		sleep(1);
		if(totalbytes == numTotal)
			break;
	}
	return 0;
}

void* thread_get_timer(void *arg)
{
	while(1)
	{
		
		printf("Transfer status : send [%s][%d%, %d KB/ %d KB]\n", fileName, numTotal_get*100/totalbytes_get, numTotal_get, totalbytes_get);
		sleep(1);
		if(totalbytes_get == numTotal_get)
			break;
	}
	return 0;
}
