#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf, FILE *fp);

int main(int argc, char *argv[])
{
    FILE *fp;
    int sock;
    pid_t pid;
    char buf[BUF_SIZE];
    struct sockaddr_in serv_adr;
    if(argc!=3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock=socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_adr.sin_port=htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
        error_handling("connect() error!");
    //프로세스의 복사본 생성
    pid=fork();
    //자식 프로세스
    if(pid==0)
        write_routine(sock, buf, fp);
    else //부모 프로세스
        read_routine(sock, buf);

    close(sock);
    return 0;
}

void read_routine(int sock, char *buf)
{
    while(1)
    {
        int str_len=read(sock, buf, BUF_SIZE);
        if(str_len==0)
            return;

        buf[str_len]=0;
        printf("Message from server: %s", buf);
    }
}
void write_routine(int sock, char *buf, FILE *fp)
{
    char END[129] = "end";
    int i, numRead;
    int retcode;
    int totalbytes;//file size
    char fileName[129];
    int fNameIndex = 0;
    while(1)
    {
	scanf("%s", buf);
        if(!strcmp(buf,"q\n") || !strcmp(buf,"Q\n"))
        {
            shutdown(sock, SHUT_WR);
            return;
        }//if
        else if(!strcmp(buf, "put") )
        {
		scanf("%s", buf);

                //file name
                for(i=0;i<strlen(buf);i++){
                     if(buf[i] == '[' || buf[i] == ']')
                         continue;
                     fileName[fNameIndex++] = buf[i];
                 }//for
		
	         fileName[fNameIndex++] = '\0';
        	 printf("%s \n", fileName);
	         
		//write 1 file name
        	 write(sock, fileName, 128);
	         
		 //file I/O
        	 fp = fopen(fileName,"rb");
	         if(fp == NULL){
                 	printf("no file \n");
                 	continue;
         	 }

           	 //file size
                 fseek(fp,0,SEEK_END);
                 totalbytes = ftell(fp);
                 printf("[%s](size:%d MB) is being sent \n",fileName,totalbytes);
                 
		//write 2 file_size
                 write(sock, &totalbytes, sizeof(int));
                 rewind(fp);
                 //send data to server
                 while(1){
                            numRead = fread(fileName,1,128,fp);
                            if(numRead > 0){
                                //send packet to server
                                //write 3 send file
                    		write(sock, fileName, numRead);
                                //usleep(1000);
                                if(retcode <= -1){

                                }
                            }
                            else{
				write(sock, END, 4);
				printf("%s \n", END);
				printf("numRead : %d \n", numRead);
				printf("break \n");
                                break;
			    }
                 }//send while
	 fclose(fp);
         }//else if put
         //기존
        // write(sock, buf, strlen(buf));
    }//while
	
}
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

