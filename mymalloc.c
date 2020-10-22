#include "mymalloc.h"
#include <stdio.h>
#define MEMORY_SIZE 4096

static unsigned char myblock[MEMORY_SIZE];

// cast size and inuse int into unsigned char | merge size and inuse into 2 bytes (16 bits) with 12 bits for size and 1 bit for inuse. 
// inuse bit is placed at front and size at the end | place the 2 bytes directly into the unsigned char currBlock array.
void numToChar(int size, int inuse, unsigned char* currBlock) {
    unsigned char FF = ('-' / 3) * ('"' / 2);       // hex: FF | binary: 1111 1111
    unsigned char eight0 = '@' * 2;               // hex: 80 | binary: 1000 0000
    unsigned char O0 = 'a' * 0;                   // hex: 00 | binary: 0000 0000

    // mask the last 8 bits of the size int with logical AND 
    currBlock[1] = FF & size;

    // merge inuse bit with the front 4 bits of size int using bit shifts and logical OR. Metadata byte formatted like x000 yyyy where x is inuse bit and yyyy are size int
    if (inuse == 1) {
        currBlock[0] = eight0 | (size >> 8);
    }
    else {
        currBlock[0] = O0 | (size >> 8);
    }
}

int charToSize(unsigned char* both) {
    unsigned char OF = '<' / 4;
    return (*(both)&OF) * 256 + *(both + 1);
}

int charToInUse(unsigned char* first) {
    if (*(first) >> 7 == 1) {
        return 1;
    }
    else {
        return 0;
    }
}

void printMem() {
    int metaSize;
    int metaUse;
    int index = 0;
    while (index < MEMORY_SIZE) {

        metaSize = charToSize(myblock + index);
        metaUse = charToInUse(myblock + index);

        if (metaSize == 0) {
            break;
        }
        printf("Metadata: 2 bytes \nBlock Size: %d bytes \nTotal Block Size: %d \nIn Use: %s\n\n", metaSize, metaSize + 2, metaUse ? "True" : "False");
        index += metaSize + 2;
    }
}
void* mymalloc(size_t size, const char* file, int line) {
    if (size == 0) {
        return myblock; //return a non null pointer?  
    }
    int metaSize;
    int metaUse;
    int index = 0;
    while (index < MEMORY_SIZE) {
        metaSize = charToSize(myblock + index);
        metaUse = charToInUse(myblock + index);
        // check current block for in use, 
        // if not in use and malloced size fits in block
        // set meta data to in use and resize
        if (metaUse == 0) {
            if (size <= metaSize) {
                numToChar(size, 1, myblock + index);
                // check if space before next block is big enough for malloc size
                if (metaSize - size >= 2 && index + 2 + size < MEMORY_SIZE - 2) {
                    //printf("%d %d\n", metaSize, index);
                    numToChar(metaSize - size - 2, 0, myblock + index + 2 + size);
                }
                // or else donate
                return myblock + index + 2; //pointer to 
            }
        }
        // if you exceed the bounds of memory
        if (index + 2 + metaSize > MEMORY_SIZE) {
            //printf("%d %d\n", index, index + 2 + metaSize);
            numToChar(MEMORY_SIZE - index - 2, 0, myblock + index);
            return myblock + index + 2;
        }
        index += 2;
        index += metaSize;
    }


    printf("error u fool\n");
    return myblock;

}

void myfree(void* pointer, const char* file, int line) {
    unsigned char F = 15;  // hex: F | binary: 0000 1111

    if ((uintptr_t)pointer >= (uintptr_t)myblock && (uintptr_t)pointer < (uintptr_t)myblock + (uintptr_t)MEMORY_SIZE) {
        int inUse = charToInUse((unsigned char*)pointer - 2);
        unsigned char* currMeta = (unsigned char*)pointer - 2;
        if (inUse) {
            currMeta[0] = currMeta[0] & F;
        }
        else {
            printf("Free Error: Memory Region Not In Use\n");
        }
    }
    else {
        printf("Free Error: Variable Not in Heap\n");
    }

    unsigned char* prevBlock = myblock;
    int prevSize = charToSize(prevBlock);
    int prevUse = charToInUse(prevBlock);
    int index = prevSize+2;
    while (index < MEMORY_SIZE) {
        if (prevSize == 0) {
            break;
        }
        int currBlock = myblock + index;
        int currSize = charToSize(currBlock);
        int currUse = charToInUse(currBlock);
        
        if (prevUse == 0 && currUse == 0) {
            numToChar(prevSize + currSize + 2,0,prevBlock);
            prevSize = prevSize + currSize + 2;
        }
        else {
            prevSize = currSize;
            prevUse = currUse;
            prevBlock = currBlock;
        }
        index += currSize + 2;
    }
}

int main() {
    numToChar(MEMORY_SIZE - 2, 0, myblock);

    printMem();
    printf("\n");
    //printf("%d",myblock[5]);
    //mymalloc(3, __FILE__, __LINE__);
    //malloc(1);
    int* test = malloc(2333);
    int* pee1 = malloc(10);
    //free(pee1);
    int* pee2 = malloc(15);
    int* pee3 = malloc(1000);
    free(pee2);
    printMem();
    free(pee1);
    printMem();
    free(pee3);
    printMem();
    
}