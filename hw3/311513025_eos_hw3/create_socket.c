# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <errno.h>
# include <unistd.h>

# define errexit(format,arg...) exit(printf(format,##arg))

int createsock(char* port){
    int server_fd, connfd, type;
    int bReuseaddr=1;
    struct sockaddr_in serv_addr;

    struct sockaddr_in addr_client;
    socklen_t sLen = sizeof(addr_client);
    
    // create socket file descriptor
    if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&bReuseaddr,sizeof(bReuseaddr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(port));

    if (bind(server_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
    {
        errexit("Can't bind to port %d:%s\n",atoi(port),strerror(errno));
    }
    
    // qlen = 3
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    return server_fd;
}