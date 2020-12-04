#include <stdlib.h>
#include <ctype.h>
#include "specieTrieADT.h"
#include "memhelper.h"

#include <stdio.h>

#define LETTER_COUNT ('z' - 'a' + 1)
#define NODE_COUNT (LETTER_COUNT + 1)

typedef struct trieNode
{
    unsigned int count;

    struct trieNode *previous;
    int previousCharIndex;

    struct trieNode *nextNodes[NODE_COUNT];
} TNode;

struct specieTrieCDT
{
    TNode trie;
};

static int getCharIndex(char c)
{
    int a = tolower(c) - 'a';
    if (a < 0 || a > 'z' - 'a')
        return LETTER_COUNT;
    return a;
}

static char getIndexChar(int index)
{
    if (index == LETTER_COUNT)
        return ' ';
    return 'a' + index;
}

specieTrieADT newSpecieTrie()
{
    specieTrieADT s;
    if (!tryCalloc((void **)&s, 1, sizeof(struct specieTrieCDT)))
        return NULL;
    s->trie.previousCharIndex = -1;
    return s;
}

static void freeTrieRecursive(TNode *node)
{
    for (int i = 0; i < NODE_COUNT; i++)
        if (node->nextNodes[i] != NULL)
            freeTrieRecursive(node->nextNodes[i]);

    free(node);
}

void freeSpecieTrie(specieTrieADT trie)
{
    for (int i = 0; i < NODE_COUNT; i++)
        if (trie->trie.nextNodes[i] != NULL)
            freeTrieRecursive(trie->trie.nextNodes[i]);

    free(trie);
}

static TNode *addNode(TNode *prev, int charIndex)
{
    TNode *newNode;
    if (!tryCalloc((void **)&newNode, 1, sizeof(TNode)))
        return NULL;

    newNode->previous = prev;
    newNode->previousCharIndex = charIndex;

    prev->nextNodes[charIndex] = newNode;
    return newNode;
}

enum TRIE_ERR specieAddOne(specieTrieADT trie, const char *specieName)
{
    if (specieName == NULL || specieName[0] == '\0')
        return TRIE_BAD_NAME;

    TNode *prev = &(trie->trie);
    TNode *current = NULL;

    while (specieName[0] != '\0')
    {
        int currentCharIndex = getCharIndex(specieName[0]);

        current = prev->nextNodes[currentCharIndex];

        if (current == NULL && (current = addNode(prev, currentCharIndex)) == NULL)
            return TRIE_NO_MEMORY;

        specieName++;

        prev = current;
    }

    current->count++;
    return TRIE_OK;
}

static void findMostPopularRecursive(TNode *currentNode, size_t currentDepth, TNode **mostPopular, size_t *mostDepth)
{
    if (currentNode == NULL)
        return;

    if (*mostPopular == NULL || (*mostPopular)->count < currentNode->count)
    {
        *mostDepth = currentDepth;
        *mostPopular = currentNode;
    }

    for (int i = 0; i < NODE_COUNT; i++)
        findMostPopularRecursive(currentNode->nextNodes[i], currentDepth + 1, mostPopular, mostDepth);
}

char *specieMostPopular(specieTrieADT trie)
{
    TNode *mostPopular = NULL;
    size_t mostDepth = 0;

    findMostPopularRecursive(&(trie->trie), 0, &mostPopular, &mostDepth);

    if (mostPopular == NULL || mostPopular->count == 0)
        return NULL;

    char *str;
    if (!tryMalloc((void **)&str, mostDepth + 1))
        return NULL;

    TNode *node = mostPopular;
    for (int i = 0; node->previousCharIndex >= 0; i++)
    {
        str[mostDepth - i - 1] = getIndexChar(node->previousCharIndex);
        node = node->previous;
    }

    str[0] = toupper(str[0]);
    /*for (int i = 1; i < mostDepth; i++)
        if (str[i - 1] == ' ')
            str[i] = toupper(str[i]);*/

    str[mostDepth] = '\0';
    return str;
}

/*static void printRecursive(TNode *node, char *buff, size_t *buffCount)
{
    if (node == NULL)
        return;

    if (node->count != 0)
    {
        buff[*buffCount] = 0;
        printf("Especie: %d x \"%s\"\n", node->count, buff);
    }

    for (int i = 0; i < NODE_COUNT; i++)
    {
        buff[(*buffCount)++] = (i == LETTER_COUNT - 1) ? ' ' : 'a' + i;
        printRecursive(node->nextNodes[i], buff, buffCount);
        (*buffCount)--;
    }
}

void speciePrintAll(specieTrieADT trie)
{
    char buff[129];
    size_t buffCount = 0;

    printf("Printing trie:\n");
    printRecursive(&(trie->trie), buff, &buffCount);
    printf("End printing trie.\n");
}*/
