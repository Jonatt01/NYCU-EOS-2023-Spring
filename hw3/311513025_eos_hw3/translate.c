# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# define BUFFERSIZE 1024
int whichstop(char *stop){
    if(strncmp(stop,"Ba",2)==0)
        return 0;
    if(strncmp(stop,"Bi",2)==0)
        return 1;
    if(strncmp(stop,"Ne",2)==0)
        return 2;
    if(strncmp(stop,"NY",2)==0)
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

char* from_status_2_report(int array[][3]){
    int msglen = 0;
    int someone_is_waiting[9] = {0};
    char tmp_msg[BUFFERSIZE] = {0};
    char report_msg[BUFFERSIZE] = {0}; // return data
    char* result;
    char* places[] = {"Baseball Stadium","Big City","Neiwan Old Street","NYCU","Smangus","17 km of Splendid Coastline","100 Tastes","Science Park","City God Temple"};
    for(int i=0;i<9;i++){
        if((array[i][0]) | (array[i][1]) | (array[i][2]))
            someone_is_waiting[i] = 1;
    }
    // printf("In from status to report\n");
    // for(int i=0;i<9;i++){
    //     printf("someone is waiting [%d] : %d",i,someone_is_waiting[i]);
    // }
    // printf("\n");
    for(int i=0;i<9;i++){
        if(someone_is_waiting[i] != 0){
            printf("someone is waiting [%d] : %d\n",i,someone_is_waiting[i]);

            msglen = sprintf(tmp_msg,"%s |",places[i]);
            printf("Place : %s\n",tmp_msg);

            strcat(report_msg, tmp_msg);
            // printf("report msg : %s\n",report_msg);
            for(int i=0;i<BUFFERSIZE;i++) tmp_msg[i]=0;  // reset the buffer
           
            if(array[i][0] != 0){
                if((array[i][1] != 0) | (array[i][2] != 0)){
                    sprintf(tmp_msg," Child %d |",array[i][0]);
                }
                else{
                    sprintf(tmp_msg," Child %d\n",array[i][0]);
                }
                strcat(report_msg,tmp_msg);
                for(int i=0;i<BUFFERSIZE;i++) tmp_msg[i]=0;  // reset the buffer
            }
            if(array[i][1] != 0){
                if((array[i][2] != 0)){
                    sprintf(tmp_msg," Adult %d |",array[i][1]);
                }
                else
                    sprintf(tmp_msg," Adult %d\n",array[i][1]);
                strcat(report_msg,tmp_msg);
                for(int i=0;i<BUFFERSIZE;i++) tmp_msg[i]=0;  // reset the buffer
            }
            if(array[i][2] != 0){
                sprintf(tmp_msg," Elder %d\n",array[i][2]);
                strcat(report_msg,tmp_msg);
                for(int i=0;i<BUFFERSIZE;i++) tmp_msg[i]=0;  // reset the buffer
            }
            // printf("report_msg in translate :%s\n",report_msg);
        }
    }
    result = report_msg;
    return result;
}