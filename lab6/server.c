# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <errno.h>
# include <unistd.h>
# include <sys/sem.h>
# include <signal.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/shm.h>

# ifdef USE_SEM
# define SEM_KEY 1122334455 
# define SEM_MODE 0666
# define BUFSIZE 1024
# define SHMSZ 27

int aquire(int s);
int release(int s);
int createsem(char *program);
int removesem(char *program, int s);
# endif

# define errexit(format,arg...) exit(printf(format,##arg))
int createsock(char* port);
void childfunc(int connfd);
void zombie_handler(int signum);
void interrupt_handler(int signum);

int money = 0; // cannot use global variable, because different process do not share the global variable
int semaphore; // for semaphore
int shmid; // for shared memory
int *shm; // for shared memory
int server_fd, connfd; // for socket

int main(int argc, char* argv[]){
    
    struct sockaddr_in addr_client;
    socklen_t sLen = sizeof(addr_client);

    
    int childpid; // for child

    char buffer[2048];

    if (signal(SIGINT, interrupt_handler) == SIG_ERR)
        fprintf(stderr, "Failed to caught SIGINT signal\n");
    if (signal(SIGCHLD, zombie_handler) == SIG_ERR)
        fprintf(stderr, "Failed to caught SIGCHLD signal\n");

    if (argc!=2)
    {
        printf("Please usage: %s <port>\n", argv[0]);
        exit(-1);
    }

    // create socket
    server_fd = createsock(argv[1]);
    if(server_fd < 0){
        perror("Error create socket");
        exit(-1);
    }

    /*semaphore*/
    semaphore = createsem(argv[0]);
    if (semaphore < 0)
    {
        perror("Error: unable to create semaphore");
        exit(-1);
    }
    if (semctl(semaphore, 0, SETVAL, 1) < 0)
    {
        perror("Error: unable to set initial value 1 to semaphore");
        exit(-1);
    }

    /* begin of shared memory initialization */
    key_t shm_key;
    shm_key = 5678; // named our shared memory segment "5678"

    // create shared memory
    if((shmid = shmget(shm_key,SHMSZ, IPC_CREAT | 0666)) < 0){
        perror("shmget");
        exit(1);
    }
    // attach the segment to our data space
    if((shm = shmat(shmid, NULL, 0)) == (int *) -1){
        perror("shmat");
        exit(1);
    }
    printf("Server create and attach the share memory.\n");
    *shm = 0;
    printf("initial value in shared memory : %d\n", *shm);
    /* end of shared memory initialization */

    while(1){

        // listen to socket
        connfd = accept(server_fd,(struct sockaddr *)&addr_client, &sLen);
        if (connfd==-1)
        {
            perror("Error: accept()\n");
            exit(-1);
        }

        // fork child process to handle this client
        childpid=fork();

        if (childpid>=0){
            if (childpid == 0){
                // read from buffer and adjust the money in atm
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
    }

    // signal(SIGCHLD,zombie_handler); // wait for child process
    // signal(SIGINT,interrupt_handler); // for crtl^c

    close(connfd);
    close(server_fd);

    return 0;
}

# ifdef USE_SEM
int aquire(int s){

    struct sembuf sop; // the operation parameters
    sop.sem_num = 0;
    sop.sem_op = -1; // if no IPC_NOWAIT flag, the process will wait until there is a resource
    sop.sem_flg = 0;

    if(semop(s,&sop,1)<0){
        fprintf(stderr, "wait(): semop failed: %s\n",strerror(errno));
        return -1;
    }
    return 0;
}

int release(int s){

    struct sembuf sop;
    sop.sem_num = 0;
    sop.sem_op = 1;
    sop.sem_flg = 0;

    if(semop(s,&sop,1)<0){
        fprintf(stderr, "wait(): semop failed: %s\n",strerror(errno));
        return -1;
    }
    return 0;
}


int createsem(char *program){

    int val = 1;
    int s;
    s = semget(SEM_KEY,1,IPC_CREAT | IPC_EXCL | SEM_MODE);
    if (s < 0)
    {
        perror("Error: unable to create semaphore");
        exit(-1);
    }
    if (semctl(s, 0, SETVAL, 1) < 0)
    {
        perror("Error: unable to set initial value 1 to semaphore");
        exit(-1);
    }

    printf("Semaphore %d has been initialized to %d\n",SEM_KEY,val);
    return s;
}
# endif

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
    printf("Socket opened successfully.\n");
    return server_fd;
}

void childfunc(int connfd)
{
    int n,val,ret,last;
    char rcv[BUFSIZE];
    char buffer[100]={0};
    while (1)
    {
        memset(rcv,0,BUFSIZE);
        memset(buffer, 0, 100);
        if ((n=read(connfd,rcv,BUFSIZE))==-1)
        {
            perror("Error: read()\n");
            exit(-1);
        } 
        printf("rcv : %s\n",rcv);

        aquire(semaphore);
        val=atoi(rcv); // string to integer
        
        // money+=val;
        *shm += val;


        if (val>0)
        {
            printf("After deposit: %d\n",*shm);
        }else if(val<0)
        {
            printf("After withdraw: %d\n",*shm);
        }

        release(semaphore);        
        if(n==0)
        {
            break;
        }

   
    }
}


void zombie_handler(int signum){
    while (waitpid(-1,NULL,WNOHANG)>0);
}

void interrupt_handler(int signum){
    printf("in the interrupt\n");
    close(connfd);
    close(server_fd);

    shmdt(shm);
    int retval = shmctl(shmid, IPC_RMID, NULL);
    if(retval < 0){
        fprintf(stderr, "Server remove shared memory failed.\n");
    }

    // delete semphore
    int s=semget(SEM_KEY,1,0);
    if(s>=0){
        if(semctl(s,0,IPC_RMID,0)<0){
            fprintf(stderr, "unable to remove semaphore");
        }
    }
}