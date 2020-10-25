CC=gcc
CFLAGS=-Wall
OUTPUTS=output *.o

main: mymalloc.o memgrind.c 
	$(CC) $(CFLAGS) mymalloc.o memgrind.c -o memgrind
mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -c mymalloc.c
clean:
	rm memgrind
	rm $(OUTPUTS)