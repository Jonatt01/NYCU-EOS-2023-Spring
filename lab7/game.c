#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <signal.h> 
#include <sys/time.h>
#include <sys/shm.h> 
#include <sys/types.h>
#include <sys/wait.h>
#define errexit(format,arg...) exit(printf(format,##arg))
typedef struct {
    int guess;
    char result[8];
}data;
sig_atomic_t shmid;
sig_atomic_t answer;
void interrupt_handler(int signal_number);
void sig_handler(int signal_number);

int main(int argc, char *argv[])
{
    key_t key;
    struct sigaction sa;


    if (argc!=3)
    {
        printf("Please usage: %s <key> <answer>\n", argv[0]);
        exit(-1);
    }
    key=atoi(argv[1]);
    answer=atoi(argv[2]);

    printf("[game] Game PID: %d\n", getpid());

    if (signal(SIGINT, interrupt_handler) == SIG_ERR)
    {
        fprintf(stderr, "Failed to caught SIGINT signal\n");
    }
        
    //create share memory
    if ((shmid = shmget(key, sizeof(data), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget");
        exit(-1);
    }

    //register sig_handler to SIGUSR1
    memset(&sa,0,sizeof(sa));
    sa.sa_handler=sig_handler;
    sigaction(SIGUSR1,&sa,NULL);
    /*sa.sa_flags=SA_SIGINFO;
    sa.sa_sigaction=sig_handler;
    sigaction(SIGUSR1,&sa,NULL);*/

    while (1);

    return 0;
}
void sig_handler(int signal_number)
{
    data *shm;
    //printf("Caught signal: %d\n", signal_number);
    if ((shm = shmat(shmid, NULL, 0)) == (void *) -1)
    {
        perror("shmat");
        exit(-1);
    }

    printf("[game] Guess %d, ",shm->guess);

    if (answer==shm->guess)
    {
        strcpy(shm->result,"bingo");
    }else if(answer<shm->guess)
    {
        strcpy(shm->result,"smaller");
    }
    else if (answer>shm->guess)
    {
        strcpy(shm->result,"bigger");
    }

    printf("%s\n",shm->result);

    shmdt(shm);

}



void interrupt_handler(int signal_number)
{
    //printf("in the interrupt\n");
    int retval = shmctl(shmid, IPC_RMID, NULL);
    if (retval < 0)
    {
        perror("Error shmctl\n");
        exit(-1);
    }
    exit(0);

}