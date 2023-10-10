# include <stdio.h>
# include <stdlib.h>

void printArray(int *array, int length){
    printf("[Status Vector]\n");
    printf("=================\n");
    for(int i=0; i<length ; i++){
        printf("%d ",*(array+i));
    }

    printf("\n");
    printf("=================\n");
}