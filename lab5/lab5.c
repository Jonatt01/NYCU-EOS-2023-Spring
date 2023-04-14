#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <netdb.h>
#define BUFSIZE 1024
#define errexit(format,arg...) exit(printf(format,##arg))
int passivesock(const char *service,const char *transport,int qlen);
void handler(int signum);
pid_t childpid;
int main(int argc, char *argv[]){

    int sockfd,connfd,i=0;
    struct sockaddr_in addr_cln;
    char snd[BUFSIZE],rcv[BUFSIZE];
    socklen_t sLen = sizeof(addr_cln);

    if (argc!=2){
        printf("Please usage: %s <port>\n", argv[0]);
        exit(-1);
    }

    sockfd=passivesock(argv[1],"tcp",10);

    if (sockfd < 0){
        perror("Error create socket\n");
        exit(-1);
    }

    while (1){
        connfd=accept(sockfd,(struct sockaddr *)&addr_cln, &sLen);
        if (connfd==-1){
            perror("Error: accept()\n");
            continue;
        }

        //process
        childpid=fork();

        if (childpid>=0){
            if (childpid == 0){
                dup2(connfd, STDOUT_FILENO);
                close(connfd);
                printf("here\n");
                execlp("/usr/games/sl","sl","-l",NULL); // 1st arg is absolute path, relative path can not work

                exit(0);
            }
            else{
                printf("Train ID: %d\n", (int)childpid);
            } 
        }
        else{
            perror("fork");
            exit(0);
        }
        
        close(connfd);
    }
    signal(SIGCHLD,handler);

    close(sockfd);

    return 0;
}
int passivesock(const char *service,const char *transport,int qlen){

    struct servent *pse;
    struct sockaddr_in sin;
    int s,type,yes=1;

    memset(&sin,0,sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_addr.s_addr=INADDR_ANY;

    if ((pse=getservbyname(service,transport))){
        sin.sin_port=htons(ntohs((unsigned short)pse->s_port));
    }
    else if ((sin.sin_port=htons((unsigned short)atoi(service)))==0){
        errexit("Can't find %s service entry\n",service);
    }
    
    if (strcmp(transport,"udp")==0){
        type=SOCK_DGRAM;
    }
    else{
        type=SOCK_STREAM;
    }

    s=socket(PF_INET,type,0);
    if (s<0){
        errexit("Can't create socket:%s\n",strerror(errno));
    }

    setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes));

    if (bind(s,(struct sockaddr *)&sin,sizeof(sin))<0){
        errexit("Can't bind to port %s:%s\n",service,strerror(errno));
    }
    
    if (type=SOCK_STREAM && listen(s,qlen)<0){
        errexit("Can't listen on port %s:%s\n",service,strerror(errno));
    }
    
    return s;
}
void handler(int signum){
    while (waitpid(-1,NULL,WNOHANG)>0);
}