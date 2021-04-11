#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#define CHUNK 1024
int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char message[2000] , server_reply[2000];

    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );

    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");
    printf( "Type 'q' to end the session:\n");
    while(1)
    {
        if( read(sock , server_reply , 2000) < 0)
        {
            puts("recv failed");
            break;
        }
        puts("Server reply : ");
        puts(server_reply);
        printf("\n");
        bzero(server_reply,CHUNK);
        sleep(5);
        printf("Enter the qn number:");
        scanf("%[^\n]%*c" , message);
        if( message[0] == 'q')
        {
             close(sock);
             return 0;
        }

        if( send(sock , message , strlen(message) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
    }

    close(sock);
    return 0;
}
