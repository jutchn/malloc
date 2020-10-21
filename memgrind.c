#include "mymalloc.h"
#include <stdio.h>
#include <sys/time.h>
#include "mymalloc.h"


int main(){
    struct timeval start_time;
    int trytime = gettimeofday(&start_time, NULL);
    if(trytime == -1) {
        printf("gettimeofday() failed");
    } 
    float start = start_time.tv_sec + start_time.tv_usec;
    unsigned char* p;
    for(int i = 0; i<120; i++){
        p = (char*)malloc(1);
        free(p);
    }

    unsigned char* parray[120];
    for(int i = 0; i < 120; i++){
        parray[i] = (char*)malloc(1);
    }
    for(int i = 0; i < 120; i++){
        free(parray[i]);
    }


    
}
