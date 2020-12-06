#ifndef specieCounterADT_h
#define specieCounterADT_h
#include <stddef.h>

/* Defines errors that can be thrown by operations from specieCounterADT. */
enum SPECIE_ERR
{
    SPECIE_OK = 0,
    SPECIE_BAD_NAME,
    SPECIE_NO_MEMORY
};

typedef struct specieCounterCDT *specieCounterADT;

/* Created a new empty specieCounterADT. If memory can't be allocated, returns NULL. */
specieCounterADT newSpecieCounter();

/* Frees all the resources used by a specieCounterADT. */
void freeSpecieCounter(specieCounterADT);

/* Increments the counter of a specific specie, adding the speecie if it wasn't previously present.
** Returns the error code of the operation. */
enum SPECIE_ERR specieAddOne(specieCounterADT, const char *specieName, size_t nameLen);

/* Finds the most popular specie and returns a new string with it's name. 
** Returns NULL if no species have been registered yet. */
char *specieMostPopular(specieCounterADT);

#endif
