#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

pthread_mutex_t mutex;
int clients[20];
int n=0;

void sendtoall(char *msg,int curr)
{
    int i;
    pthread_mutex_lock(&mutex);
    for(i = 0; i < n; i++) 
    {
        //if(clients[i] != curr) 
        {
            if(send(clients[i],msg,strlen(msg),0) < 0) 
            {
                printf("sending failure \n");
                continue;
            }
        }
    }
    pthread_mutex_unlock(&mutex);
}

void *recvmg(void *client_sock)
{
    int sock = *((int *)client_sock);
    char msg[500];
    int len;
    int fd = open("cmd.txt", O_RDWR | O_CREAT , 00777);
    int fd1, fd2;
    fd2 = open("out.txt", O_RDWR | O_CREAT , 00777);
    char buffer[500] = {'\0'};

    //dup2(fd2,1);
    while((len = recv(sock,msg,500,0)) > 0) 
    {
        msg[len] = '\0';
        //printf("%s\n",msg);
        //if(strncmp(&msg[len - 2],"ls",2) == 0)
        //if(strncmp(msg,"ls",2) == 0)
        {
            dup2(fd2,1);
            fd2 = open("out.txt", O_RDWR | O_CREAT, 00777);
            fd1 = open("cmd.txt", O_RDWR | O_CREAT | O_TRUNC, 00777);
            write(fd1, msg, strlen(msg));
            system("bash cmd.txt"); 
            //remove("cmd.txt");

            read(fd2,buffer,sizeof(buffer));
            send(sock,buffer,sizeof(buffer),0);
            memset(buffer,0,sizeof(buffer));
            fd2 = open("out.txt", O_RDWR | O_CREAT | O_TRUNC, 00777);
            //remove("out.txt");
        }
        //sendtoall(msg,sock);
    }

}

int main(){
    struct sockaddr_in ServerIp;
    pthread_t recvt;
    int sock=0 , Client_sock=0;

    ServerIp.sin_family = AF_INET;
    ServerIp.sin_port = htons(8080);
    ServerIp.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock = socket( AF_INET , SOCK_STREAM, 0 );
    if( bind( sock, (struct sockaddr *)&ServerIp, sizeof(ServerIp)) == -1 )
        printf("cannot bind, error!! \n");
    else
        printf("Server Started\n");

    if( listen( sock ,20 ) == -1 )
        printf("listening failed \n");

    while(1)
    {
        if( (Client_sock = accept(sock, (struct sockaddr *)NULL,NULL)) < 0 )
            printf("accept failed  \n");
        pthread_mutex_lock(&mutex);
        clients[n]= Client_sock;
        n++;
        // creating a thread for each client 
        pthread_create(&recvt,NULL,(void *)recvmg,&Client_sock);
        pthread_mutex_unlock(&mutex);
    }
    return 0; 

}