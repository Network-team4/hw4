#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 128
void error_handling(char *message);
//void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);//, FILE *fp);

int main(int argc, char *argv[])
{
    //FILE *fp;
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
  
	write_routine(sock, buf);
   

    close(sock);
    return 0;
}

void write_routine(int sock, char *buf)//, FILE *fp)
{

    int i, numRead;
    int retcode;
    int totalbytes;//file size
    char t_msg[129];
    char last_msg[129];


	int sumRead = 0;
    while(1)
    {
	int len = 0;
        char fileName[129];
        int fNameIndex = 0;
	FILE *fp;
	printf("while start \n");
	scanf("%s", buf);

	//write 0 put or get
       	write(sock, buf, strlen(buf)+1);

	//printf("%d \n", buf);
        if(!strcmp(buf,"q") || !strcmp(buf,"Q"))
        {
            //shutdown(sock, SHUT_WR);
            break;
        }//if
        else if(!strcmp(buf, "put") )
        {


		scanf("%s", buf);
		printf("%s \n", buf);
		
                //file name
                for(i=0;i<strlen(buf);i++){
                     if(buf[i] == '[' || buf[i] == ']')
                         continue;
                     fileName[fNameIndex++] = buf[i];
                 }//for
		
	         fileName[fNameIndex++] = '\0';
        	 printf("%s \n", fileName);

		//write 1 file name
        	 write(sock, fileName, strlen(fileName)+1 );
		printf("%s \n", fileName);
	         
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
		//write 3 file transfer
		while(1){
			numRead = fread(t_msg,1,128,fp);
			printf("numRead : %d \n", numRead);
			if(numRead < 128){
				write(sock, t_msg, numRead);
				break;
			}
			usleep(100);
			write(sock, t_msg, 128);
		}
		//shutdown(sock, SHUT_WR);
		fclose(fp);
		//close(sock);
         }//else if put
	 else if(!strcmp(buf, "get") ){
		printf("get부분은 여기에 코딩 \n");
	 }	
	else{
		printf("else \n");
	}
         //기존
        // write(sock, buf, strlen(buf));
    }//while
    close(sock);
	
}
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

