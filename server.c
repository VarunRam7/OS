 #include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#define CHUNK 1024
//the thread function
void *connection_handler(void *);

int thread_count = 0;
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);


    //Accept and incoming connection

    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");

        pthread_t thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        if( pthread_create( &thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }

        puts("Thread Assigned");
    }

    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }

    return 0;
}
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    size_t nread;
    char client_message[2000],server_msg[CHUNK];
    //bzero(server_msg, strlen(server_msg));
    FILE *fp = fopen("q.txt","r");
        if(fp)
        {
                while((nread=fread(server_msg, 1, sizeof server_msg, fp))>0)
                {

                        send(sock , server_msg , sizeof server_msg , 0);
                        bzero(server_msg, sizeof server_msg);
                }
                fclose(fp);
        }
        printf("options sent\n\n ");

    //Receive a message from client
    while( (read_size = read(sock , client_message , 2000)) > 0 )
    {
        //Send the message back to client
        char answer[2000];
        int count ,c_count;
        sleep(2);
        printf("message from client  %d :\n%s\n\n",sock-3,client_message);
        c_count = atoi(client_message);
        FILE *fans = fopen("a.txt","r");
        if(fans)
        {
                count = 1;
                while(fgets(answer,sizeof(answer),fans)!=NULL)
                {
                        if(c_count == count)
                        {
                                printf("%s",answer);
                                send(sock, answer , strlen(answer),0);
                                break;
                        }
                        count++;
                }
                fclose(fans);
        }
        FILE *fp = fopen("q.txt","r");
        if(fp)
        {
                while((nread=fread(server_msg,1,sizeof server_msg,fp))>0)
                {
                        send(sock , server_msg , sizeof server_msg ,0);
                        bzero(server_msg, sizeof server_msg);
                        bzero(client_message,sizeof client_message);
                }
                fclose(fp);
        }
        printf("options sent\n");
    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    //Free the socket pointer
    free(socket_desc);
    close(sock);
    pthread_exit(NULL);
    return NULL;
}
