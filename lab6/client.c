#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define BUFSIZE 1024
  
int main(int argc, char const* argv[])
{
    // for socket
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char* hello = "Hello from client";
    char buffer[1024] = { 0 };
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
  
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if ((status= connect(client_fd, (struct sockaddr*)&serv_addr,sizeof(serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // for sending data
    char snd[BUFSIZE];
    int num;

    if (argc != 6)
    {
        printf("Please usage: %s <ip> <port> <deposit/withdraw> <amount> <times>\n", argv[0]);
        exit(-1);
    }

    if (strcmp(argv[3],"withdraw")==0)
    {
        num=atoi(argv[4]);
        num=-num;
        sprintf(buffer,"%d",num);
        //printf("%s\n",buffer);
    }else
    {
        strcpy(buffer,argv[4]);
    }
    

    for (int i = 0; i < atoi(argv[5]); i++)
    {
        write(client_fd,buffer,strlen(buffer));
        usleep(5000);
        
    }

    // closing the connected socket
    close(client_fd);
    return 0;
}