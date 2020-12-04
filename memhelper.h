#ifndef memhelper_h
#define memhelper_h

/* Provides some basic "wrappers" around memory management functions
** to help with allocating memory. */

#include <stddef.h>

/* Attempts to malloc a given amount of continuos bytes in memory.
** If the operation is successful, a pointer to the allocated memory is
** left where indicated by the "ptr" parameter and the function returns 1.
** If the operation fails, ptr is unmodified and the function returns 0. */
int tryMalloc(void **ptr, size_t size);

/* Attempts to realloc the given pointer. If the operation is successful,
** the reallocated pointer is left in ptr and the function returns 1.
** If the realloc fails, the original ptr IS FREED and 0 is returned. */
int tryRealloc(void **ptr, size_t newSize);

/* Attempts to calloc the given amount of memory and saves the allocated,
** zeroed-out memory to the location indicated by the "ptr" parameter.
** Returns 1 if successful, or 0 if the operation failed (and ptr stays unmodified). */
int tryCalloc(void **ptr, size_t nmemb, size_t size);

#endif