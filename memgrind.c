#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mymalloc.h"
int main() {
    // Initialize timing variables
    struct timeval time;
    double worktime[5] = { 0 };
    double start;
    double end;
    int trytime;

    // Run 50 iterations of each workload and average their times
    for (int w = 0; w < 50; w++) {
        // Workload A: 
        // malloc() 1 byte and free it 120 times

        // start timing
        trytime = gettimeofday(&time, NULL); 
        if (trytime == -1) { printf("gettimeofday() failed"); }
        start = time.tv_sec + (double)time.tv_usec / 1000000;

        // malloc and free 120 times
        unsigned char* p;
        for (int i = 0; i < 120; i++) {
            p = (unsigned char*)malloc(1);
            free(p);
        }

        //end timing
        trytime = gettimeofday(&time, NULL); 
        if (trytime == -1) { printf("gettimeofday() failed"); }
        end = time.tv_sec + (double)time.tv_usec / 1000000;
        worktime[0] += (end - start);


        // Workload B: 
        // malloc() 1 byte, store pointer in array
        // after 120 calls to malloc(), free all pointers

        // start timing
        trytime = gettimeofday(&time, NULL); 
        if (trytime == -1) { printf("gettimeofday() failed"); }
        start = time.tv_sec + (double)time.tv_usec / 1000000;

        // malloc and store 120 times then free 120 times
        unsigned char* parray2[120];
        for (int i = 0; i < 120; i++) {
            parray2[i] = (unsigned char*)malloc(1);
        }
        for (int i = 0; i < 120; i++) {
            free(parray2[i]);
        }
        
        //end timing
        trytime = gettimeofday(&time, NULL); 
        if (trytime == -1) { printf("gettimeofday() failed"); }
        end = time.tv_sec + (double)time.tv_usec / 1000000;
        worktime[1] += end - start;


        // Workload C:  
        // 240 times, randomly choose between a 1 byte malloc()
        // or free()ing one of the malloc()ed pointers

        // start timing
        trytime = gettimeofday(&time, NULL);
        if (trytime == -1) { printf("gettimeofday() failed"); }
        start = time.tv_sec + (double)time.tv_usec / 1000000;

        // do 120 mallocs and 120 frees randomly
        unsigned char* parray3[120];
        int nummalloc = 0;
        int maxmalloc = 120;
        int totalmalloc = 0;
        int freed;

        void doMalloc() {
            parray3[nummalloc] = (unsigned char*)malloc(1);
            nummalloc++;
            totalmalloc++;
        }
        void doFree() {
            freed = rand() % nummalloc;
            free(parray3[freed]);
            parray3[freed] = parray3[nummalloc - 1];
            nummalloc--;
        }
        for (int i = 0; i < 240; i++) {
            if (nummalloc == 0) {
                doMalloc();
            }
            else if (totalmalloc == maxmalloc) {
                doFree();
            }
            else {
                if (rand() % 2 == 1) {
                    doMalloc();
                }
                else {
                    doFree();
                }
            }
        }

        //end timing
        trytime = gettimeofday(&time, NULL);
        if (trytime == -1) { printf("gettimeofday() failed"); }
        end = time.tv_sec + (double)time.tv_usec / 1000000;
        worktime[2] += end - start;


        // Workload D: 
        // Fill memory with 2 byte blocks and free every other block
        // Then malloc into those spaces to fill memory again and free everything

        // start timing
        trytime = gettimeofday(&time, NULL);
        if (trytime == -1) { printf("gettimeofday() failed"); }
        start = time.tv_sec + (double)time.tv_usec / 1000000;

        int iterations = 1024;
        unsigned char* parray4[iterations];
        for (int i = 0; i < iterations; i++) {
            parray4[i] = (unsigned char*)malloc(2);
        }
        for (int i = 0; i < iterations / 2; i += 2) {
            free(parray4[i]);
        }
        for (int i = 0; i < iterations / 2; i += 2) {
            parray4[i] = (unsigned char*)malloc(2);
        }
        for (int i = 0; i < iterations; i++) {
            free(parray4[i]);
        }

        // end timing
        trytime = gettimeofday(&time, NULL);
        if (trytime == -1) { printf("gettimeofday() failed"); }
        end = time.tv_sec + (double)time.tv_usec / 1000000;
        worktime[3] += end - start;


        // Workload E: 
        // malloc 2 bytes 1020 times, free blocks that aren't multiples of 5 in 1,2,4,3 order
        // Finally malloc 14 bytes to fill up memory and free everything

        // start timing
        trytime = gettimeofday(&time, NULL);
        if (trytime == -1) { printf("gettimeofday() failed"); }
        start = time.tv_sec + (double)time.tv_usec / 1000000;

        iterations = 1020;
        unsigned char* parray5[iterations];
        for (int i = 0; i < iterations; i++) {
            parray5[i] = (unsigned char*)malloc(2);
        }
        for (int i = 0; i < iterations; i += 5) {
            free(parray5[i + 1]);
            free(parray5[i + 2]);
            free(parray5[i + 4]);
            free(parray5[i + 3]);
        }
        for (int i = 0; i < iterations; i += 5) {
            parray5[i + 1] = (unsigned char*)malloc(14);
        }
        for (int i = 0; i < iterations; i += 5) {
            free(parray5[i]);
            free(parray5[i + 1]);
        }

        // end timing
        trytime = gettimeofday(&time, NULL);
        if (trytime == -1) { printf("gettimeofday() failed"); }
        end = time.tv_sec + (double)time.tv_usec / 1000000;
        worktime[4] += end - start;
    }

    for (int j = 0; j < 5; j++) {
        worktime[j] = worktime[j] / 50;
        printf("Mean runtime for workload %d: %.15lf seconds | %.5lf microseconds\n", j + 1, worktime[j], worktime[j] * 1000000);
    }
}
