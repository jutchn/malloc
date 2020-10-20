#include "mymalloc.h"
#include <stdio.h>

static unsigned char myblock[4096];

// cast size and inuse int into unsigned char | merge size and inuse into 2 bytes (16 bits) with 12 bits for size and 1 bit for inuse. 
// inuse bit is placed at front and size at the end | place the 2 bytes directly into the unsigned char myblock array.
void numToChar(int size, int inuse, unsigned char *myblock){
    unsigned char FF = ('-'/3) * ('"'/2);       // hex: FF | binary: 1111 1111
    unsigned char eight0 = '@'*2;               // hex: 80 | binary: 1000 0000
    unsigned char O0 = 'a'*0;                   // hex: 00 | binary: 0000 0000

    // mask the last 8 bits of the size int with logical AND 
    myblock[1] =  FF & size;

    // merge inuse bit with the front 4 bits of size int using bit shifts and logical OR. Metadata byte formatted like x000 yyyy where x is inuse bit and yyyy are size int
    if(inuse == 1){
        myblock[0] = eight0 | (size>>8);
    } else {
        myblock[0] = O0 | (size>>8);
    }
    //printf("%d %d\n",myblock[0],myblock[1]);
}

int charToSize(unsigned char *both){
    unsigned char OF = '<'/4;
    printf("%d\n",(*(both) & OF))<<8;
    return (*(both) & OF)*256 + *(both + 1);
}

int charToInUse(unsigned char *first){
    if(*(first)>>7 == 1){
        return 1;
    } else {
        return 0;
    }
}

void printMem(unsigned char *myblock){
    int i = 0;
    while(1){
        int temp;
    }
}
void* mymalloc(int size, const char* file, int line){
    if(size == 0){
        return -1; //return a non null pointer?  
    }
    int metaSize;
    int metaUse;    
    int index = 0;
    while(index < 4096){
        metaSize = charToSize(myblock + index);
        metaUse = charToSize(myblock + index);
        
        if(metaUse == 0){
            if(size<=metaSize){
                numToChar(size,1,myblock + index);
                return index; //pointer to 
            }
        }
        index+=2;
        index+=metaSize;
    }
    if(4096 - index >= size + 2){
        numToChar(size, 1, myblock + index);
        return index;
    }

    perror("error u fool");
    return -1;

}

int myfree(){

}

int main(){
    numToChar(0,0,myblock);


}
