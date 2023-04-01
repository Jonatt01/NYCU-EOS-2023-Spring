# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>


int main(int argc, char *argv[]){

    char* name;
    int len_of_name = 0;

    name = argv[1];
    printf("%s\n", name);
    int fd = open("/dev/mydev",O_RDWR);

    if(fd<0){
        printf("%s","Can not open the file");
    }

    // assume the length of name no longer than 10
    for(int i=0;i<8;i++){

        if( write(fd,&name[i],1) <0){
            perror("Error: "); // include in stdio.h
            exit(EXIT_FAILURE); // # define EXIT_FAILURE 1 in stdlib.h
        }

        printf("%c\n",name[i]);
        sleep(1); // include in unistd.h
    }


    return EXIT_SUCCESS; // // # define EXIT_FAILURE 0 in stdlib.h

}
