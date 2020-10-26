#define malloc( x ) mymalloc( x, __FILE__, __LINE__ )
#define free( x ) myfree( x, __FILE__, __LINE__ )
#include <stddef.h>

void* mymalloc(size_t size, char* file, int line);

void myfree(void* pointer, char* file, int line);