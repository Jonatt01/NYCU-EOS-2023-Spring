# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>

int main(int argc, char *argv[]){

    char* number;

    number = argv[1];
    printf("%s", number);
    int fd = open("/dev/etx_device",O_WRONLY);
    if(fd<0){
        printf("%s","Can not open the file");
    }
    write(fd,number,10);
    
    return 0;

}
