#ifndef DEBUG_H
#define DEBUG_H

#ifdef EBUG

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void* DEBUG_MEMmalloc(size_t size, char* file, int line);
void DEBUG_MEMfree(void* ptr, char* file, int line);
void* DEBUG_MEMrealloc(void* old_ptr,size_t new_size, char* file, int line);
void DEBUG_MEMeval();
#define malloc(X)	 DEBUG_MEMmalloc(X,__FILE__,__LINE__)
#define free(X)		 DEBUG_MEMfree(X,__FILE__,__LINE__)
#define realloc(X,Y) DEBUG_MEMrealloc(X,Y,__FILE__,__LINE__)


#define DEBUG_EVAL(){DEBUG_MEMeval();}
#define exit(X) {DEBUG_EVAL(); exit(X);} ((void)0)

#ifdef __cplusplus
}
#endif
	
#endif

#endif /* DEBUG_H */