#ifndef specieTrieADT_h
#define specieTrieADT_h

/* Defines errors that can be thrown by operations from specieTrieADT. */
enum SPECIE_ERR
{
    SPECIE_OK = 0,
    SPECIE_BAD_NAME,
    SPECIE_NO_MEMORY
};

typedef struct specieTrieCDT *specieTrieADT;

/* Created a new empty specieTrieADT. If memory can't be allocated, returns NULL. */
specieTrieADT newSpecieTrie();

/* Frees all the resources used by a specieTrieADT. */
void freeSpecieTrie(specieTrieADT);

/* Increments one to the counter of a specific specie. Returns the error code of the operation. */
enum SPECIE_ERR specieAddOne(specieTrieADT, const char *specieName);

/* Finds the most popular specie in the trie and returns a new string with it's name.
** Returns NULL if the trie is empty or if memory couldn't be allocated. */
char *specieMostPopular(specieTrieADT);

#endif
