#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8888
  
int main(int argc, char const* argv[])
{
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char* hello = "Hello from client";
    char buffer[1024] = { 0 };
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
  
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if ((status= connect(client_fd, (struct sockaddr*)&serv_addr,sizeof(serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }


    send(client_fd, "Menu", strlen("Menu")+1, 0);
    printf("Menu message sent\n");
    valread = read(client_fd, buffer, 1024);
    printf("%s\n", buffer);
    
    for(int i=0;i<1024;i++) buffer[i]=0;

    // send(client_fd, "Search", strlen("Search")+1, 0);
    // printf("Search message sent\n");    
    // valread = read(client_fd, buffer, 1024);
    // printf("%s\n", buffer);

    // for(int i=0;i<1024;i++) buffer[i]=0;

    // send(client_fd, "Search | Big City", strlen("Search | Big City")+1, 0);
    // printf("Search message sent\n");    
    // valread = read(client_fd, buffer, 1024);
    // printf("%s\n", buffer);

    // for(int i=0;i<1024;i++) buffer[i]=0;


    send(client_fd, "Report | Big City | Adult 5 | Child 6 | Baseball Stadium | Adult 3", strlen("Report | Big City | Adult 5 | Child 6 | Baseball Stadium | Adult 3")+1, 0);
    printf("Report message sent\n");
    valread = read(client_fd, buffer, 1024);
    printf("%s\n", buffer);

    for(int i=0;i<1024;i++) buffer[i]=0;

    valread = read(client_fd, buffer, 1024);
    printf("%s\n", buffer);

    for(int i=0;i<1024;i++) buffer[i]=0;

    send(client_fd, "Report | Smangus | Adult 6 | Big City | Child 5 | Adult 5 | NYCU | Child 6", strlen("Report | Smangus | Adult 6 | Big City | Child 5 | Adult 5 | NYCU | Child 6"), 0);
    printf("2 Report message sent\n");    
    valread = read(client_fd, buffer, 1024);
    printf("%s\n", buffer);

    for(int i=0;i<1024;i++) buffer[i]=0;

    valread = read(client_fd, buffer, 1024);
    printf("%s\n", buffer);

    for(int i=0;i<1024;i++) buffer[i]=0;

    // send(client_fd, "Search | Big City", strlen("Search | Big City"), 0);
    // printf("Search message sent\n");    
    // valread = read(client_fd, buffer, 1024);
    // printf("%s\n", buffer);

    // for(int i=0;i<1024;i++) buffer[i]=0;

    // send(client_fd, "Report | Big City | Adult -1 | 100 Tastes | Child 4 | City God Temple | Elder 1", strlen("Report | Big City | Adult -1 | 100 Tastes | Child 4 | City God Temple | Elder 1"), 0);
    // printf("3 Report message sent\n");    
    // valread = read(client_fd, buffer, 1024);
    // printf("%s\n", buffer);

    // for(int i=0;i<1024;i++) buffer[i]=0;

    // valread = read(client_fd, buffer, 1024);
    // printf("%s\n", buffer);

    // for(int i=0;i<1024;i++) buffer[i]=0;

    // send(client_fd, "Search | Big City", strlen("Search | Big City"), 0);
    // printf("Search message sent\n");    
    // valread = read(client_fd, buffer, 1024);
    // printf("%s\n", buffer);

    // for(int i=0;i<1024;i++) buffer[i]=0;

    // send(client_fd, "Search", strlen("Search"), 0);
    // printf("Search message sent\n");    
    // valread = read(client_fd, buffer, 1024);
    // printf("%s\n", buffer);

    // for(int i=0;i<1024;i++) buffer[i]=0;

    send(client_fd, "Exit", strlen("Exit")+1, 0);
    printf("Exit message sent\n");
  
    // closing the connected socket
    close(client_fd);
    return 0;
}