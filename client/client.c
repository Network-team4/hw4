#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int BUF_SIZE = 128;

void error_handling(char *message);
//void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);//, FILE *fp);

int main(int argc, char *argv[])
{
    //FILE *fp;
    int sock;
    pid_t pid;
    char buf[1024];
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
    int vSend=0;
    int i, numRead;
    int retcode;
     int numTotal;
	int totalbytes,totalbytes_get;//file size

    char last_msg[129];


	int sumRead = 0;
    while(1)
    {
	int sumBytes=0;
	int len = 0;
        char fileName[129];
        int fNameIndex = 0;
	FILE *fp;
	printf("while start \n");
	printf(">파일보내기(put)\n>파일받기(get)\n>파일전송비율(sendrate)\n>파일수신비율(recvrate)\n>파일송수신비율보기(ratecurr)\n>(credit)\n>종료(q or Q)\n");
	scanf("%s", buf);
	int flag[11]={0};

	//write 0 put or getnnn
       	write(sock, buf, strlen(buf)+1);

	//printf("%d \n", buf);
        if(!strcmp(buf,"q") || !strcmp(buf,"Q"))
        {
            //shutdown(sock, SHUT_WR);
            break;
        }//if
        else if(!strcmp(buf, "put") )
        {
		printf("보낼 파일명을 입력하세요\n 예시 : [파일명.파일형식]\n");
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
                 printf("[%s](size:%d MB) is being sent \n",fileName, totalbytes);
                 
		//write 2 file_size
                 write(sock, &totalbytes, sizeof(int));
                 rewind(fp);
                 //send data to server		
		//write 3 file transfer
		printf("[");
		while(1){
			int percentage;
			char *tu ="*";
			char progress[11];
			
			numRead = fread(buf,1,BUF_SIZE,fp);
			//printf("numRead : %d \n", numRead);
			//프로그래스바
			sumBytes += numRead;
			percentage = (float)sumBytes / totalbytes * 100;
			if(flag[(int)percentage / 10] == 0 && percentage / 10 != 0){
				printf("%s", tu);
				//usleep(1000);
				fflush(stdout);
				flag[percentage/10] = 1;
			}

			if(numRead < BUF_SIZE){
				write(sock, buf, numRead);
				break;
			}
			usleep(1000);
			write(sock, buf, BUF_SIZE);
		}//while
		printf("]\n");
		printf("Successfully transferred \n");
		//shutdown(sock, SHUT_WR);
		fclose(fp);
		//close(sock);
         }//else if put
	 else if(!strcmp(buf, "get") ){
		printf("받은 파일명을 입력하세요\n 예시 : [파일명.파일형식]\n");
		scanf("%s", buf);
			printf("%s \n", buf);

			//file name
			for (i = 0; i < strlen(buf); i++){
				if (buf[i] == '[' || buf[i] == ']')
					continue;
				fileName[fNameIndex++] = buf[i];
			}//for

			fileName[fNameIndex++] = '\0';
			printf("%s \n", fileName);

			//write 1 file name
			write(sock, fileName, strlen(fileName) + 1);
			printf("%s \n", fileName);

			//read 1 file size
			read(sock, &totalbytes_get, sizeof(int));
			printf("file size : %d \n", totalbytes_get);

			FILE *pFile = fopen(fileName, "wb");
			//receiving a TCP packet
			fp = fopen(fileName, "ab");
			printf("[");
			fflush(stdout);
			do{
				int percentage;
				char *tu ="*";
				char progress[11];
				numRead = read(sock, buf, 128);
				sumBytes += numRead;
				percentage = (float)sumBytes / totalbytes_get * 100;
				if(flag[(int)percentage / 10] == 0 && percentage / 10 != 0){
					printf("%s", tu);
					//usleep(1000);
					fflush(stdout);
					flag[percentage/10] = 1;
				}
				fwrite((void*)buf, 1, numRead, pFile);
//				fclose(fp);
			//	printf("numRead : %d \n", numRead);
			//	numTotal += numRead;
			//	printf("numtotal : %d \n", numTotal);

			} while ((numRead == 128) && (numTotal != totalbytes_get));
			printf("]\n");
			printf("Successfully transferred \n");
			fclose(pFile);
			printf("finish \n");
	 }
	 else if(!strcmp(buf, "sendrate") ){	
		scanf("%s", buf);
		vSend = atoi(buf);
		printf("%d \n", vSend);
		BUF_SIZE = vSend;
		write(sock, &vSend, sizeof(int) );

	 }
	 else if(!strcmp(buf, "ratecurr") ){
		printf("send : %dK, recv : \n", BUF_SIZE);
	 }
	 else if(!strcmp(buf, "credit") ){

		while(1)
		{
			int stdID;
			printf("학번 입력(종료(0)) : ");
			scanf("%d",&stdID);
			if(stdID == 20115233){
				printf("20115233 김학균\n");
				printf("<-- client.c --> \n");
				printf("1. 멀티프로세스를 이용한 서버 다중접속이 가능하도록 밑바탕 구현 \n");
				printf("2. 파일을 전송하는 put부분 전체 구현 \n");
				printf("3. sendrate, 종료(Q), ratecurr 그리고 credit 등을 구현하였습니다.\n"); 
				printf("<-- server.c --> \n");
				printf("1. 멀티프로세스 방식으로 여러 클라이언트가 하나의 서버에 접속 가능하도록 밑바탕 구현 \n");
				printf("2. put, sendrate, Q 그리고 1초마다 얼만큼 클라이언트로부터 파일을 전송받고 있는지를 나타내는 것을 쓰레드로 구현 				thread_get_timer함수\n");
			}
			else if(stdID == 20135181){
				printf(“20135181 김진하\n”);
				printf(“progress bar interface 구현\n”);
			}
			else if(stdID == 20133221){
				printf("20133221 박세희\n");
				printf("<-- client.c --> \n");
				printf("파일을 전송하는 get부분 전체 구현 \n ");
				printf("<-- server.c --> \n");
				printf("get 전체 구현 \n");
			}
			else if(stdID == 0) break;
		}
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

