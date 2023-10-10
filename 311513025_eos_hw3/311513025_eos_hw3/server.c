# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <errno.h>
# include <unistd.h>
# include <semaphore.h>
# include <signal.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/shm.h>
# include <sys/mman.h>
# include <sys/stat.h>        /* For mode constants */
# include <fcntl.h>           /* For O_* constants */

# include "create_socket.h"
# include "array_operation.h"
# include "translate.h"

# define SEM_KEY 1234567890
# define SEM_MODE 0666
# define SHMSZ 27
# define BUFFERSIZE 1024

/**
*global variables
*/
int server_fd, connfd;
sem_t *data_sem;
sem_t *bus_sem;
const char *shm_name = "STATUS";
const char *report_shm_name = "WHO_REPORTED";
int *status;
int *report_status; // for checking if it is possible to merge to another process

typedef struct{
    int flag; // whether there is someone waiting
    pid_t pid[10]; // save the pid for someone also waiting
} report_list;

void interrupt_handler(int signum);
void zombie_handler(int signum);
void childfunc(int connfd);

int main(int argc, char* argv[]){


    struct sockaddr_in addr_client;
    socklen_t sLen = sizeof(addr_client);

    int childpid; // for child

    server_fd = createsock(argv[1]);
    printf("Socket create successfully.\n");
    
    // for interrupt
    if (signal(SIGINT, interrupt_handler) == SIG_ERR)
        fprintf(stderr, "Failed to caught SIGINT signal\n");
    if (signal(SIGCHLD, zombie_handler) == SIG_ERR)
        fprintf(stderr, "Failed to caught SIGCHLD signal\n");

    // POSIX semaphore for waiting information
    data_sem = sem_open("/SEM_DATA", O_CREAT, 0666, 1);
    if(data_sem == SEM_FAILED)  
    {  
        perror("Data_sem init failed:");  
        return -1;  
    }
    // POSIX semaphore for waiting information
    bus_sem = sem_open("/SEM_BUS", O_CREAT, 0666, 2);
    if(bus_sem == SEM_FAILED)  
    {  
        perror("Bus_sem init failed:");  
        return -1;  
    }  

    // create shared memory
    int shm_fd, shm_report_fd;

    /* open the shared memory segment */
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        printf("shared memory failed\n");
        exit(-1);
    }
    /* configure the size of the shared memory segment */
    ftruncate(shm_fd,27*sizeof(int));
    /* now map the shared memory segment in the address space of the process */
    status = (int *) mmap(0,27*sizeof(int), PROT_READ | PROT_WRITE , MAP_SHARED, shm_fd, 0);
    if (status == MAP_FAILED) {
        printf("Map failed\n");
        exit(-1);
    }
    for(int i = 0; i < 27; i++) *(status+i) = 0;

    shm_report_fd = shm_open(report_shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_report_fd == -1) {
        printf("shared memory failed\n");
        exit(-1);
    }
    ftruncate(shm_fd,9*sizeof(report_list));
    report_status = (int *) mmap(0,9*sizeof(report_list), PROT_READ | PROT_WRITE , MAP_SHARED, shm_report_fd, 0);
    if (report_status == MAP_FAILED) {
        printf("Map failed\n");
        exit(-1);
    }


    while(1){

        connfd = accept(server_fd,(struct sockaddr *)&addr_client, &sLen); // listening to socket

        if (connfd==-1)
        {
            perror("Error: accept()\n");
            exit(-1);
        }
        else{
            printf("new connection\n");
        }

        // fork child process to handle this client
        childpid=fork();

        if (childpid>=0){
            if (childpid == 0){
                childfunc(connfd);
            }
            else{
                continue;
            }
        }
        else{
            perror("fork");
            exit(-1);
        }
        close(connfd);
    }
    
    // delete named POSIX semphore
    sem_unlink("/SEM_DATA");
    sem_unlink("/SEM_BUS");
    close(server_fd);
    close(connfd);

    return 0;
}

void childfunc(int connfd){
    
    char snd[BUFFERSIZE] = {0}, rcv[BUFFERSIZE] = {0};
    int msglen = 0;
    int val_data, val_bus; //a tmp use for checking semaphore value

    
    while(1){

        memset(rcv,0,BUFFERSIZE);// set the rcv char array to zero
        memset(snd,0,BUFFERSIZE);// set the snd char array to zero

        // read the message to rcv buffer array
        if ((read(connfd,rcv,BUFFERSIZE))==-1){
            perror("Error: read()\n");
            exit(-1);
        }
        if(strlen(rcv) != 0){
            printf("rcv msg from %d : %s\n",connfd,rcv);
        }
        else{
            // strlen(rcv) == 0
            close(connfd);
            break;
        }

        sem_wait(data_sem);
        sem_getvalue(data_sem,&val_data);
        printf("Outer %d , data sem value=%d, pid=%d\n",connfd, val_data, getpid());

        if(strncmp(rcv,"Menu",4)==0){
            sem_post(data_sem);
            // sem_getvalue(data_sem,&val_data);
            // printf("Menu %d , data sem value=%d, pid=%d\n",connfd, val_data, getpid());

            // return the number of bytes that are written in the array (not counting the ending null character)
            msglen = sprintf(snd,"1. Search\n2. Report\n3. Exit\n");
            printf("snd : %s\n",snd);

            if ((write(connfd,snd,msglen+1))==-1){
                perror("Error: write()\n");
                exit(-1);
            }
        }
        else if(strncmp(rcv,"Search",6)==0){
            char *token;
            int wait_count[9] = {0};
            for(int i=0;i<9;i++){
                wait_count[i] = status[i*3] +status[i*3 + 1]+status[i*3 + 2];
            }
            printArray(status, 27);
            
            // specific stop
            if(strncmp(rcv,"Search |",8)==0){
                token=strtok(rcv,"|"); // include string.h
                // printf("\ntoken : %s\n",token);
                token=strtok(NULL,"|");
                // printf("\ntoken : %s\n",token);
                while(token != NULL){
                    
                    int stop = 0;
                    char tmp[64]; // save the stop before removing space
                    strcpy(tmp, token+1); // copy without the first space
                    remove_spaces(token);

                    stop = whichstop(token);
                    printf("\nstop : %d\n",stop);
                    msglen = sprintf(snd,"%s - Child : %d | Adult : %d | Elder : %d\n",tmp,status[stop*3 + 0],status[stop*3 + 1],status[stop*3 + 2]);
                    write(connfd,snd,msglen+1);
                    token=strtok(NULL,"|");
                    // printf("\ntoken : %s\n",token);

                    sem_post(data_sem);
                    // sem_getvalue(data_sem,&val_data);
                    // printf("Search %d Getvalue: value=%d, pid=%d\n\n",connfd, val_data, getpid());
                }

            }
    
            // all stop
            else{
                msglen = sprintf(snd,"1. Baseball Stadium : %d\n2. Big City : %d\n3. Neiwan Old Street : %d\n4. NYCU : %d\n5. Smangus : %d\n6. 17 km of Splendid Coastline : %d\n7. 100 Tastes : %d\n8. Science Park : %d\n9. City God Temple : %d\n"
                                ,wait_count[0],wait_count[1],wait_count[2],wait_count[3],wait_count[4],wait_count[5],wait_count[6],wait_count[7],wait_count[8]);
                if ((write(connfd,snd,msglen+1))==-1){
                    perror("Error: write()\n");
                    exit(-1);
                }

                sem_post(data_sem);
                // sem_getvalue(data_sem,&val_data);
                // printf("Search %d Getvalue: value=%d, pid=%d\n\n",connfd, val_data, getpid());
            }
        }
        else if(strncmp(rcv,"Report",6)==0){

            int now_report[9][3] = {0}; // the whole information in ths report

            char *token;
            char temp_msg[BUFFERSIZE];
            char reportrtn[BUFFERSIZE]; //tmp_msg for each parse, reportrtn save the whole message
            int max_stop = 0; // who dominates the waiting time

            // sem_wait(bus_sem);
            // sem_getvalue(bus_sem,&val_bus);
            // printf("Report (after wait) %d , bus sem value=%d, pid=%d\n",connfd, val_bus, getpid());

            msglen = sprintf(snd, "The shuttle bus is on it's way...\n");
            if ((write(connfd,snd,msglen+1))==-1){
                perror("Error: write()\n");
                exit(-1);
            }

            // printf("Now report\n");
            // for(int i=0;i<9;i++){
            //     printf("%d %d %d\n",now_report[i][0],now_report[i][1],now_report[i][2]);
            // }
            // parse the report
            token=strtok(rcv,"|");
            printf("\ntoken : %s\n",token);
            token=strtok(NULL,"|"); // get the stop
            printf("token (stop) : %s\n",token);
            while(token != NULL){
                int stop = 0; // which stop (index) is this parse
                char target[10]; // which stop in this parse
                int inc_dec_num = 0;
                char tmp[64]; // save the stop before removing space
                int age=0;

                for(int i=0;i<BUFFERSIZE;i++) temp_msg[i]=0;  // reset the buffer
                
                // parse the stop part
                strcpy(tmp, token+1);
                remove_spaces(token);
                stop = whichstop(token);
                printf("STOP: %d",stop);
                if(stop>=max_stop) max_stop = stop;

                // parse the age and people count part
                token=strtok(NULL,"|"); // get the target and num
                do{
                    printf("--------------------------\n");
                    sscanf(token," %s %d",target,&inc_dec_num); // save the age to target, save the waiting people count in inc_dec_num
                    printf("%s : %d\n",target,inc_dec_num);
                    age = whichage(target);
                    now_report[stop][age] += inc_dec_num; // only useed in this report for sorting
                    *(status + stop*3 + age) += inc_dec_num; // change global waiting status
                    token = strtok(NULL,"|"); // next target num or next stop
                    printf("token2 : %s\n",token);
                    if(token == NULL)
                        break;
                }while(strncmp(token," Child",6)==0 | strncmp(token," Adult",6)==0 | strncmp(token," Elder",6)==0);
            }
            printf("Now report\n");
            for(int i=0;i<9;i++){
                printf("%d %d %d\n",now_report[i][0],now_report[i][1],now_report[i][2]);
            }
            printArray(status,27);

            // check if there is possibility to merge
            
            

            // after the signal happens (whether from another process or me)
            // send the message to client
            char* tmp = from_status_2_report(now_report);
            strcpy(reportrtn,tmp);
            printf("In test : %s\n",reportrtn);

            sem_post(data_sem);
            sem_getvalue(data_sem,&val_data);
            printf("Report %d , data sem value=%d, pid=%d\n",connfd, val_data, getpid());

            sleep(max_stop+1);
            if ((write(connfd,reportrtn,strlen(reportrtn)+1))==-1){
                perror("Error: write()\n");
                exit(-1);
            }



            // sem_post(bus_sem);
            // sem_getvalue(bus_sem,&val_bus);
            // printf("Report (after post) %d , bus sem value=%d, pid=%d\n",connfd, val_bus, getpid());

        }

        else if(strncmp(rcv,"Exit",4)==0){
            sem_post(data_sem);
            // sem_getvalue(data_sem,&val_data);
            // printf("Exit %d Getvalue: value=%d, pid=%d\n",connfd, val_data, getpid());

            // closing the connected socket
            close(connfd);
            break;
        }
    }

}

void zombie_handler(int signum){
    while (waitpid(-1,NULL,WNOHANG)>0);
}

void interrupt_handler(int signum){

    // avoid to use printf in signal handler
    write(STDOUT_FILENO, "In the interrupt\n", 17);
    close(connfd);
    close(server_fd);
    
    int err = munmap(status, 27*sizeof(int));
    if(err != 0){
        write(STDOUT_FILENO, "UnMapping Failed\n", 17);
        exit(-1);
    }
    err = munmap(report_status, 9*sizeof(report_list));
    if(err != 0){
        write(STDOUT_FILENO, "UnMapping Failed\n", 17);
        exit(-1);
    }

    /* ISSUE : remove the shared memory segment */
    // if (shm_unlink(shm_name) == -1) {
    //     printf("Error removing %s\n",shm_name);
    //     exit(-1);
    // }

    // delete named POSIX semphore
    sem_unlink("/SEM_DATA");
    sem_unlink("/SEM_BUS");
    
    // exit(0);

}