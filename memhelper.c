#include "memhelper.h"
#include <stdlib.h>
#include <ctype.h>

int tryMalloc(void **ptr, size_t size)
{
    void *newPtr = malloc(size);
    if (newPtr == NULL)
        return 0;

    *ptr = newPtr;
    return 1;
}

int tryRealloc(void **ptr, size_t newSize)
{
    void *newPtr = realloc(*ptr, newSize);
    if (newPtr == NULL)
    {
        free(*ptr);
        return 0;
    }

    *ptr = newPtr;
    return 1;
}

int tryCalloc(void **ptr, size_t nmemb, size_t size)
{
    void *newPtr = calloc(nmemb, size);
    if (newPtr == NULL)
        return 0;

    *ptr = newPtr;
    return 1;
}
