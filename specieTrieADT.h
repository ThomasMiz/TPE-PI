#ifndef specieTrieADT_h
#define specieTrieADT_h

/* Defines errors that can be thrown by operations from specieTrieADT. */
enum TRIE_ERR
{
    TRIE_OK = 0,
    TRIE_BAD_NAME,
    TRIE_NO_MEMORY
};

typedef struct specieTrieCDT *specieTrieADT;

/* Created a new empty specieTrieADT. If memory can't be allocated, returns NULL. */
specieTrieADT newSpecieTrie();

/* Frees all the resources used by a specieTrieADT. */
void freeSpecieTrie(specieTrieADT);

/* Increments one to the counter of a specific specie. Returns the error code of the opration. */
enum TRIE_ERR specieAddOne(specieTrieADT, const char *specieName);

/* Finds the most popular specie in the trie and returns a new string with it's name.
** Returns NULL if the trie is empty or if memory couldn't be allocated. */
char *specieMostPopular(specieTrieADT);

/* Prints all the elements in the trie, indicating how many ocurrences each one has. */
//void speciePrintAll(specieTrieADT);

#endif
