#ifndef specieTrieADT_h
#define specieTrieADT_h

// Defines errors that can be thrown by operations from specieTrieADT.
enum TRIE_ERR
{
    TRIE_OK = 0,
    TRIE_BAD_NAME,
    TRIE_NO_MEMORY
};

typedef struct specieTrieCDT *specieTrieADT;

// Created a new empty specieTrieADT.
specieTrieADT newSpecieTrie();

// Frees all the resources used by a specieTrieADT.
void freeSpecieTrie(specieTrieADT);

// Increments one to the counter of a specific specie.
enum TRIE_ERR specieAddOne(specieTrieADT, const char *specieName);

// Finds the most popular specie in the trie and returns a new string with it's name.
char *specieMostPopular(specieTrieADT);

//void speciePrintAll(specieTrieADT);

#endif