#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

char msg[500];

void *recvmg(void *my_sock)
{
    int sock = *((int *)my_sock);
    int len;
    // client thread always ready to receive message
    while((len = recv(sock,msg,500,0)) > 0) 
    {
        msg[len] = '\0';
        
        fputs(msg,stdout);
    }
}

int main(int argc,char *argv[]){
    pthread_t recvt;
    int len;
    int sock;
    char send_msg[500];
    struct sockaddr_in ServerIp;
    char client_name[100];
    strcpy(client_name, argv[1]);
    sock = socket( AF_INET, SOCK_STREAM,0);
    ServerIp.sin_port = htons(8080);
    ServerIp.sin_family= AF_INET;
    ServerIp.sin_addr.s_addr = inet_addr("127.0.0.1");
    if( (connect( sock ,(struct sockaddr *)&ServerIp,sizeof(ServerIp))) == -1 )
        printf("\n connection to socket failed \n");

    //creating a client thread which is always waiting for a message
    pthread_create(&recvt,NULL,(void *)recvmg,&sock);

    //ready to read a message from console
    while(fgets(msg,500,stdin) > 0) 
    {
        msg[strlen(msg) - 1] = '\0';
        //strcpy(send_msg,client_name);
        //strcat(send_msg,":");
        //strcat(send_msg,msg);
        if(strncmp(msg,"get-mem",7) == 0 )
        {

            strcpy(send_msg,"top -bn 1 | grep ");
            strcat(send_msg,&msg[8]);
            //printf("s: %ld, l: %ld", sizeof(send_msg), strlen(send_msg));
            //send_msg[strlen(send_msg)+ 1] = '\0';
            strcat(send_msg," | cut -d ' ' -f 17");
            //printf("%s",send_msg);
            //strcpy(send_msg,msg);
            len = write(sock,send_msg,strlen(send_msg));

        }
        else if(strncmp(msg,"get-cpu-usage",13) == 0 )
        {

            strcpy(send_msg,"top -bn 1 | grep ");
            strcat(send_msg,&msg[14]);
            //printf("s: %ld, l: %ld", sizeof(send_msg), strlen(send_msg));
            //send_msg[strlen(send_msg)+ 1] = '\0';
            strcat(send_msg," | cut -d ' ' -f 15");
            //printf("%s",send_msg);
            //strcpy(send_msg,msg);
            len = write(sock,send_msg,strlen(send_msg));

        }
        else
        {
            strcpy(send_msg,msg);
            len = write(sock,send_msg,strlen(send_msg));
        }
        
        
        if(len < 0) 
            printf("\n message not sent \n");
    }

    //thread is closed
    pthread_join(recvt,NULL);
    close(sock);
    return 0;
}