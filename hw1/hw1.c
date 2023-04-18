# include <stdio.h>
# include <stdlib.h>
# include <unistd.h> 
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <string.h>
# include <stdio.h>

int total_waiting[3][3]; // [stop][age]

int main(){
    int first_choice = 0;

    // opening LED device
    int fd_led = open("/dev/etx_device",O_WRONLY);
    if (fd_led < 0){
        perror("Error opening LED device");
        exit(EXIT_FAILURE);
    }

    // opening 7 seg device
    int fd_seg = open("/dev/mydev",O_WRONLY);
    if (fd_seg < 0){
        perror("Error opening 7 seg device");
        exit(EXIT_FAILURE);
    }

    while(1){
        printf("1. Search\n");
        printf("2. Report\n");
        printf("3. Exit\n");
        
        scanf("%d",&first_choice);

        if(first_choice == 1){
            while(1){
                // print the information on the screen
                int section_count[3]; // [baseball ,big city, zoo] waiting peoole count
                for(int i=0;i<3;i++){
                    section_count[i] = total_waiting[i][0]+total_waiting[i][1]+total_waiting[i][2];
                }
                printf("1. Baseball Stadium : %d\n", section_count[0]);
                printf("2. Big City : %d\n", section_count[1]);
                printf("3. Zoo : %d\n", section_count[2]);

                // write to the two drivers
                // int buf[3]={section_count[0],section_count[1],section_count[2]};
                char charbuf[3]={section_count[0]+'0',section_count[1]+'0',section_count[2]+'0'};

                if(write(fd_led,charbuf,3) < 0){
                    perror("error led write");
                    exit(EXIT_FAILURE);
                }
                int tmp[1] = {0}; // save the total waiting people in this section to send to sev_seg
                tmp[0] = section_count[0] + section_count[1] + section_count[2];
                if(write(fd_seg,tmp,1) < 0){
                    perror("error seg write");
                    exit(EXIT_FAILURE);
                }

                char second_choice;
                // next input
                scanf("%s", &second_choice);
                if(second_choice == 'q'){
                    printf("Break out\n");
                    
                    // write to led, to turn off
                    char close_led[3] = {'0','0','0'};

                    if(write(fd_led,close_led,3) < 0){
                        perror("error led write");
                        exit(EXIT_FAILURE);
                    }
                    
                    // write to seg, to turn off
                    char close_7seg[1] = {'/'}; 
                    if(write(fd_seg,close_7seg,1) < 0){
                        perror("error seg write");
                        exit(EXIT_FAILURE);
                    }
                    
                    break;
                }
                else{
                    int area = second_choice - '0'; // char to int#

                    // write to led driver
                    for(int i=0;i<3;i++){
                        if(i == area -1){
                            charbuf[i] = '/';
                        }
                        else{
                            charbuf[i] = '0';
                        }
                    }
                    
                    if(write(fd_led,charbuf,3) < 0){
                        perror("error led write");
                        exit(EXIT_FAILURE);
                    }


                    printf("Child : %d\n", total_waiting[area-1][0]);
                    printf("Adult : %d\n", total_waiting[area-1][1]);
                    printf("Elder : %d\n", total_waiting[area-1][2]);

                    // write to seven segment
                    tmp[0] = total_waiting[area-1][0] + total_waiting[area-1][1] + total_waiting[area-1][2];
                    if(write(fd_seg,tmp,1) < 0){
                        perror("error seg write");
                        exit(EXIT_FAILURE);
                    }

                    scanf("%s", &second_choice); // just for waiting one input no other meaning                   
                }
            }
        }

        if(first_choice ==2){
            while(1){
                int area=0;
                char addorminus;
                char group;
                int incdec_num=0;
                
                printf("Area (1~3) : ");
                scanf("%d", &area);

                // write to led
                char charbuf[3] = {'0','0','0'};
                charbuf[area-1] = '1';
                if(write(fd_led,charbuf,3) < 0){
                    perror("error led write");
                    exit(EXIT_FAILURE);
                }
                
                // write to 7 seg (before changing)
                int tmp[1] = {0};
                tmp[0] = total_waiting[area-1][0] + total_waiting[area-1][1] + total_waiting[area-1][2];
                if(write(fd_seg,tmp,1) < 0){
                    perror("error seg write");
                    exit(EXIT_FAILURE);
                }

                printf("Add or Reduce ('a' or 'r') : ");
                scanf(" %c", &addorminus);
                printf("Age group ('c' or 'a' or 'e') : ");
                scanf(" %c", &group);
                printf("The number of passenger : ");
                scanf("%d",&incdec_num);


                // adjust the total waiting passenger number of the area
                int intgroup;
                if(group == 'c')intgroup=0;
                if(group == 'a')intgroup=1;
                if(group == 'e')intgroup=2;
                printf("%c\n",addorminus);

                if(addorminus == 'a'){
                    total_waiting[area-1][intgroup] += incdec_num;
                }
                else{
                    total_waiting[area-1][intgroup] -= incdec_num;
                }

                // write to 7 seg (after changing)
                tmp[0] = total_waiting[area-1][0] + total_waiting[area-1][1] + total_waiting[area-1][2];
                if(write(fd_seg,tmp,1) < 0){
                    perror("error seg write");
                    exit(EXIT_FAILURE);
                }


                char exit_or_continue;
                printf("Exit ('e') or Continue ('c') : ");
                scanf("%s",&exit_or_continue);
                if(exit_or_continue == 'e'){
                    printf("Break from Reporting !\n");
                    
                    // write to led, to turn off
                    char close_led[3] = {'0','0','0'};
                    if(write(fd_led,close_led,3) < 0){
                        perror("error led write");
                        exit(EXIT_FAILURE);
                    }
                    
                    // write to seg, to turn off
                    char close_7seg[1] = {'/'}; 
                    if(write(fd_seg,close_7seg,1) < 0){
                        perror("error seg write");
                        exit(EXIT_FAILURE);
                    }
                    
                    break;
                }
                else if(exit_or_continue == 'c'){
                    printf("Continue to Report !\n");
                    continue;
                }
            }
        }

        if(first_choice == 3){
            char close_led[3] = {'0','0','0'};
            char close_7seg[1] = {'/'}; 

            // write to led
            if(write(fd_led,close_led,3) < 0){
                perror("error led write");
                exit(EXIT_FAILURE);
            }

            // write to 7 seg
            if(write(fd_seg,close_7seg,1) < 0){
                perror("error seg write");
                exit(EXIT_FAILURE);
            }
            break;
        }
    }
    return EXIT_SUCCESS;
}
