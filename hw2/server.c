# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <errno.h>
# include <unistd.h>

# define PORT 8080
# define BUFFERSIZE 1024
#define errexit(format,arg...) exit(printf(format,##arg))
int createsock(char* port);
int whichstop(char *stop);
void remove_spaces(char *stop);
int whichage(char *target);

int main(int argc, char* argv[]){

    int server_fd, connfd;
    struct sockaddr_in addr_client;
    socklen_t sLen = sizeof(addr_client);

    int waiting[9][3]={0}; // [stop][age]
    int wait_count[9]={0}; // totol waiting num in each area
    char snd[BUFFERSIZE] = {0},rcv[BUFFERSIZE] = {0};

    int msglen;

    server_fd = createsock(argv[1]);

    // while loop for keeping server open
    while(1){
        // initialize for each connection
        for(int i=0;i<9;i++){
            waiting[i][0] = 0;
            waiting[i][1] = 0;
            waiting[i][2] = 0;
        }

        // accept the cilent connection
        connfd = accept(server_fd,(struct sockaddr *)&addr_client, &sLen);
        if(connfd==-1){
            perror("Error: accept()\n");
            exit(-1);
        }
        else{
            printf("new connection\n");
        }

        // while loop for one clinet connection
        while(1){
            
            // for(int i=0;i<BUFFERSIZE;i++) snd[i]='0';

            // for debugging
            for(int i=0;i<9;i++){
                printf("%d %d %d\n",waiting[i][0],waiting[i][1],waiting[i][2]);
            }

            memset(rcv,0,BUFFERSIZE);// set the rcv char array to zero
            memset(snd,0,BUFFERSIZE);// set the snd char array to zero
            
            // read the message to rcv buffer array
            if ((read(connfd,rcv,BUFFERSIZE))==-1){
                perror("Error: read()\n");
                exit(-1);
            }
            printf("rcv : %s\n",rcv);
            
            if(strncmp(rcv,"Menu",4)==0){
                // return the number of bytes that are written in the array (not counting the ending null character)
                msglen = sprintf(snd,"1. Search\n2. Report\n3. Exit\n");
                printf("snd : %s\n",snd);

                if ((write(connfd,snd,msglen))==-1){
                    perror("Error: write()\n");
                    exit(-1);
                }
            }

            if(strncmp(rcv,"Search",6)==0){
                char *token;
                
                for(int i=0;i<9;i++){
                    wait_count[i] = waiting[i][0]+waiting[i][1]+waiting[i][2];
                }
                
                // specific stop
                if(strncmp(rcv,"Search |",8)==0){
                    token=strtok(rcv,"|"); // include string.h
                    // printf("\ntoken : %s\n",token);
                    token=strtok(NULL,"|");
                    // printf("\ntoken : %s\n",token);
                    while(token != NULL){
                        
                        int stop = 0;
                        char tmp[64]; // save the stop before removing space
                        strcpy(tmp, token+1);
                        remove_spaces(token);

                        stop = whichstop(token);
                        printf("\nstop : %d\n",stop);
                        msglen = sprintf(snd,"%s - Child : %d Adult : %d Elder : %d",tmp,waiting[stop][0],waiting[stop][1],waiting[stop][2]);
                        write(connfd,snd,msglen);
                        token=strtok(NULL,"|");
                        // printf("\ntoken : %s\n",token);
                    }

                }
        
                // all stop
                else{
                    msglen = sprintf(snd,"1. Baseball Stadium : %d\n2. Big City : %d\n3. Neiwan Old Street : %d\n4. NYCU : %d\n5. Smangus : %d\n6. 17 km of Splendid Coastline : %d\n7. 100 Tastes : %d\n8. Science Park : %d\n9. City God Temple : %d\n"
                                    ,wait_count[0],wait_count[1],wait_count[2],wait_count[3],wait_count[4],wait_count[5],wait_count[6],wait_count[7],wait_count[8]);
                    if ((write(connfd,snd,msglen))==-1){
                        perror("Error: write()\n");
                        exit(-1);
                    }
                }
            }
            else if(strncmp(rcv,"Report",6)==0){
                char *token;
                char temp_msg[BUFFERSIZE],reportrtn[BUFFERSIZE]={0};
                int max_stop = 0;

                token=strtok(rcv,"|");
                printf("\ntoken (stop) : %s\n",token);
                token=strtok(NULL,"|"); // get the stop
                printf("\ntoken (stop) : %s\n",token);
                while(token != NULL){
                    int stop = 0;
                    char target[10];
                    int inc_dec_num = 0;
                    char tmp[64]; // save the stop before removing space
                    int age=0;

                    for(int i=0;i<BUFFERSIZE;i++) temp_msg[i]=0;  // reset the buffer
                    
                    strcpy(tmp, token+1);
                    remove_spaces(token);
                    stop = whichstop(token);
                    if(stop>=max_stop) max_stop = stop;

                    token=strtok(NULL,"|"); // get the target and num
                    printf("\ntoken (target and num) : %s\n",token);
                    sscanf(token," %s %d",target,&inc_dec_num);
                    printf("%s : %d\n",target,inc_dec_num);

                    // prepare the message to send to cliient
                    sprintf(temp_msg,"%s| %s %d\n",tmp,target,inc_dec_num);
                    // printf("temp_msg : %s\n",temp_msg);
                    strcat(reportrtn,temp_msg);
                    printf("reportrtn : \n%s\n",reportrtn);

                    age = whichage(target);
                    printf("waiting[%d][%d]",stop,age);

                    // adjust the waiting num in each area
                    waiting[stop][age]+= inc_dec_num;

                    token=strtok(NULL,"|"); // get the next stop
                    printf("\ntoken (stop) : %s\n",token);

                }
                
                msglen = sprintf(snd,"Please wait a few seconds...\n");
                write(connfd,snd,msglen);
                sleep(max_stop+1);
                write(connfd,reportrtn,strlen(reportrtn));


            }
            else if(strncmp(rcv,"Exit",4)==0){
                // closing the connected socket
                close(connfd);
                break;
            }
        }
    }

    // closing the listening socket
    close(server_fd);    

    return 0;

}


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

int whichstop(char *stop){
    if(strncmp(stop,"Ba",2)==0)
        return 0;
    if(strncmp(stop,"Bi",2)==0)
        return 1;
    if(strncmp(stop,"Ne",2)==0)
        return 2;
    if(strncmp(stop,"Ny",2)==0)
        return 3;
    if(strncmp(stop,"Sm",2)==0)
        return 4;
    if(strncmp(stop,"17",2)==0)
        return 5;
    if(strncmp(stop,"10",2)==0)
        return 6;
    if(strncmp(stop,"Sc",2)==0)
        return 7;
    if(strncmp(stop,"Ci",2)==0)
        return 8;
}

void remove_spaces(char* s) {
    char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}

int whichage(char *target){
    if(strncmp(target,"C",1)==0)
        return 0;
    if(strncmp(target,"A",1)==0)
        return 1;
    if(strncmp(target,"E",1)==0)
        return 2;
}