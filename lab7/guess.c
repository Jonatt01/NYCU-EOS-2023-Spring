#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <signal.h>   
#include <time.h> 
#include <sys/time.h> 
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#define errexit(format,arg...) exit(printf(format,##arg))
typedef struct {
    int guess;
    char result[8];
}data;
void time_handler(int signal_number);
sig_atomic_t shmid;
sig_atomic_t lower=0;
sig_atomic_t upper;
sig_atomic_t stop=0;
pid_t game_pid;
int main(int argc, char *argv[])
{
    key_t key;
    struct itimerval timer;
    struct sigaction sa;

    if (argc!=4)
    {
        printf("Please usage: %s <key> <upper_bound> <pid>\n", argv[0]);
        exit(-1);
    }
    key=atoi(argv[1]);
    upper=atoi(argv[2]);
    game_pid=atoi(argv[3]);

    //create share memory
    if ((shmid = shmget(key, sizeof(data), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget");
        exit(-1);
    }

    //install time_handler as the signal handler for SIGVTALRM
    memset(&sa,0,sizeof(sa));
    sa.sa_handler=time_handler;
    sigaction(SIGVTALRM,&sa,NULL);

    //set timer
    memset(&timer,0,sizeof(timer));
    //timer to 1 sec
    timer.it_value.tv_sec=1;
    timer.it_value.tv_usec=0;
    //reset timer back to 1 sec after timer expire
    timer.it_interval.tv_sec=1;
    timer.it_interval.tv_usec=0;
    //start virtual timer
    setitimer(ITIMER_VIRTUAL,&timer,NULL);

    while(!stop);

    return 0;
}
void time_handler(int signal_number)
{
    data *shm;
    //printf("time up: %d\n", signal_number);
    if ((shm = shmat(shmid, NULL, 0)) == (void *) -1)
    {
        perror("shmat");
        exit(-1);
    }

    if (strcmp(shm->result,"bingo")==0)
    {
        stop=1;
        return;
    }
    else if(strcmp(shm->result,"smaller")==0)
    {
        upper=shm->guess;
    }else if(strcmp(shm->result,"bigger")==0)
    {
        lower=shm->guess;
    }

    shm->guess=(upper+lower)/2;
    printf("[game] Guess: %d\n",shm->guess);
    
    kill(game_pid,SIGUSR1);
    shmdt(shm);
    

}