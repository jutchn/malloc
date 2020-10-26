#include <stdio.h>
#include <stddef.h>
#define MEMORY_SIZE 4096 // Global constant representing total memory size in bytes
#define METADATA_SIZE 2 // Global Constant representing metadata size in bytes 

static unsigned char myblock[MEMORY_SIZE]; // Global char array to represent memory heap

// makeMetadata() turns size and inuse values into metadata and places it into memory
// Metadata is formatted in 2 bytes with the following structure: x000 yyyy yyyy yyyy, where x is the InUse bool and y bits are the size
// This allows for a max MEMORY_SIZE of 4096 but it could be expanded to 3 bytes of the form: x000 0000 yyyy yyyy yyyy yyyy if it needs to be scaled
void makeMetadata(int size, int inuse, unsigned char* currBlock) {
    // cast size and inuse int into unsigned char and place the 2 bytes directly into the unsigned char currBlock array.
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

// Takes a pointer to a memory block's metadata and returns its Size value
int metaToSize(unsigned char* both) {
    unsigned char OF = '<' / 4;
    return (*(both)&OF) * 256 + *(both + 1);
}

// Takes a pointer to a memory block's metadata and returns its InUse boolean
int metaToInUse(unsigned char* first) {
    if (*(first) >> 7 == 1) {
        return 1;
    }
    else {
        return 0;
    }
}

// Used for testing. Function goes through memory and prints each block's metadata
void printMem() {
    int metaSize;
    int metaUse;
    int index = 0;
    while (index < MEMORY_SIZE) {

        metaSize = metaToSize(myblock + index);
        metaUse = metaToInUse(myblock + index);

        if (metaSize == 0) {
            break;
        }
        printf("Metadata: 2 bytes \nBlock Size: %d bytes \nTotal Block Size: %d \nIn Use: %s\n\n", metaSize, metaSize + 2, metaUse ? "True" : "False");
        index += metaSize + METADATA_SIZE;
    }
    printf("---------------------------------------------\n");
}

// Takes a size variable and allocates a block of memory of that size 
// along with 2 bytes of meta data that describe the block 
// Returns a pointer to the allocated block of memory
void* mymalloc(size_t size, const char* file, int line) {
    // initialize memory with metadata of block size 4094 and inuse 0
    if (metaToSize(myblock + 0) == 0) {
        makeMetadata(MEMORY_SIZE - METADATA_SIZE, 0, myblock);
    }
    if (size == 0) {
        return NULL; //If size is 0 return a null pointer  
    }
    int metaSize;
    int metaUse;
    int index = 0;
    while (index < MEMORY_SIZE) {
        metaSize = metaToSize(myblock + index);
        metaUse = metaToInUse(myblock + index);
        // check current block for in use, 
        // if not in use and malloced size fits in block
        // set meta data to in use and resize
        if (metaUse == 0) {
            if (size <= metaSize) {
                makeMetadata(size, 1, myblock + index);
                // check if space before next block is big enough for malloc size
                if (metaSize - size >= METADATA_SIZE && index + METADATA_SIZE + size < MEMORY_SIZE - METADATA_SIZE) {
                    makeMetadata(metaSize - size - METADATA_SIZE, 0, myblock + index + METADATA_SIZE + size);
                }
                // or else donate
                return myblock + index + METADATA_SIZE; //pointer to 
            }
        }
        // if you exceed the bounds of memory
        if (index + METADATA_SIZE + metaSize > MEMORY_SIZE) {
            makeMetadata(MEMORY_SIZE - index - METADATA_SIZE, 0, myblock + index);
            return myblock + index + METADATA_SIZE;
        }
        index += METADATA_SIZE;
        index += metaSize;
    }

    // If no space is available to allocate a block of size bytes, throw an error and return a null pointer
    printf("\033[1;31m");
    printf("Malloc Error: No available space to allocate (file:%s line:%d)\n", file, line);
    printf("\033[0m");
    return NULL;
}

// Takes a pointer and free the block of memory it points to, if it points to any
void myfree(void* pointer, const char* file, int line) {

    // Throw null pointer error
    if (pointer == NULL) {
        printf("\033[1;31m");
        printf("Free Error: Null Pointer (file:%s line:%d)\n", file, line);
        printf("\033[0m");
        return;
    }

    // Loop through memory until you either find the block or reach the end of memory
    int index = 0;
    int isInHeap = 0; // Boolean to represent if pointer points to block of memory
    while (index < MEMORY_SIZE) {
        unsigned char* currPointer = myblock + index;
        if ((unsigned char*)pointer == currPointer + METADATA_SIZE) {
            // Set InUse boolean to false if pointer is in memory
            if (metaToInUse(currPointer)) {
                makeMetadata(metaToSize(currPointer), 0, currPointer);
            }
            else {
                // Throw error if block being pointed to is not in use
                printf("\033[1;31m");
                printf("Free Error: Memory Region Not In Use (file:%s line:%d)\n", file, line);
                printf("\033[0m");
                return;
            }
            isInHeap = 1;
            break;
        }
        index += metaToSize(currPointer) + METADATA_SIZE;
    }
    if (!isInHeap) {
        // Throw error if pointer doesn't point to block of memory
        printf("\033[1;31m");
        printf("Free Error: Variable Not in Memory (file:%s line:%d)\n", file, line);
        printf("\033[0m");
        return;
    }

    // Loop through memory array and look for adjacent unused blocks
    unsigned char* prevBlock = myblock;
    int prevSize = metaToSize(prevBlock);
    int prevUse = metaToInUse(prevBlock);
    index = prevSize + METADATA_SIZE;
    while (index < MEMORY_SIZE) {
        unsigned char* currBlock = myblock + index;
        int currSize = metaToSize(currBlock);
        int currUse = metaToInUse(currBlock);

        if (prevUse == 0 && currUse == 0) {
            // Combine unused blocks into one block
            makeMetadata(prevSize + currSize + METADATA_SIZE, 0, prevBlock);
            prevSize = prevSize + currSize + METADATA_SIZE;
        }
        else {
            prevSize = currSize;
            prevUse = currUse;
            prevBlock = currBlock;
        }
        index += currSize + METADATA_SIZE;
    }
    return;
}