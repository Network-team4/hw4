#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
void read_childproc(int sig);

int main(int argc, char *argv[])
{
    int totalbytes;//file size
	char fileName[128];
	int i;
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	//FILE *fp;
	pid_t pid;
	struct sigaction act;
	socklen_t adr_sz;
	int str_len, state;
	char buf[129];
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
		int numTotal=0;
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
					//read 3 transfer file
					do{
						str_len = read(clnt_sock,buf,128);
						fwrite((void*)buf, 1, str_len, fp);
						printf("str_len : %d \n", str_len);
       						//nread = recvfrom(sockid,fileName,512000,0, (struct sockaddr *) &client_addr, (socklen_t*) &addrlen);
						//fwrite(fileName,1,nread,fp);
					        numTotal += str_len;
					        // printf("nread : %d \n",nread);
					         printf("numtotal : %d \n",numTotal);
					}while((str_len == 128) && (numTotal != totalbytes));

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
				}//if
				else if( !strcmp("get", buf) ){
					printf("get은 여기다 코딩하세요! \n");
				}
				else if( !strcmp("q", buf) || !strcmp("Q", buf) ){
					printf("종료합니다(Q) \n");
					close(clnt_sock);
					close(serv_sock);
					return 0;

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
