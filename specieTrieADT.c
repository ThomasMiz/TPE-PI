#include <stdlib.h>
#include <ctype.h>
#include "specieTrieADT.h"
#include "memhelper.h"

/* The amount of letters in the english alphabet. */
#define LETTER_COUNT ('z' - 'a' + 1)

/* The amount of child nodes each node in the tree has. */
#define NODE_COUNT (LETTER_COUNT + 1)

/* The lowest depth value for a node. Used for calculations. */
#define LOWEST_NODE_DEPTH 0

/* Represents a single node in the tree. */
typedef struct trieNode
{
    /* How many times a string that leads to this node has been added to the tree. */
    unsigned int count;

    /* The node that leads to this node, or NULL if this is the first node. */
    struct trieNode *previous;
    /* The character index in the previous node, in which this node is located. */
    int previousCharIndex;

    /* This node's child nodes, ordered alphabetically.
    ** When a string's char isn't a letter, the last index in this array is used. */
    struct trieNode *nextNodes[NODE_COUNT];
} TNode;

struct specieTrieCDT
{
    TNode trie;
};

/* Gets the index in the TNode.nextNodes array in which a char belongs. */
static int getCharIndex(char c)
{
    int a = tolower(c) - 'a';
    if (a < 0 || a > 'z' - 'a')
        return LETTER_COUNT;
    return a;
}

/* Gets the character that leads to the specified index in the TNode.nextNodes array. */
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

/* Recursively goes through all the nodes and subnodes, freeing all of them. */
static void freeTrieRecursive(TNode *node)
{
    for (int i = 0; i < NODE_COUNT; i++)
        if (node->nextNodes[i] != NULL)
            freeTrieRecursive(node->nextNodes[i]);

    free(node);
}

void freeSpecieTrie(specieTrieADT trie)
{
    // Since the first node is contained inside the specieTrieCDT, we can't
    // free() it (it gets freed when we free the struct specieTrieCDT).
    // So we recursively free all of it's children, then free the CDT.
    for (int i = 0; i < NODE_COUNT; i++)
        if (trie->trie.nextNodes[i] != NULL)
            freeTrieRecursive(trie->trie.nextNodes[i]);

    free(trie);
}

/* Creates a single node and adds it to the tree, setting is "previous" field
** and placing the new node in the correct index in the previous node's nextNode.
** If memory can't be allocated, returns NULL. Otherwise, returns the new node. */
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

enum SPECIE_ERR specieAddOne(specieTrieADT trie, const char *specieName)
{
    // We first check that we received a non-null-or-empty string.
    if (specieName == NULL || specieName[0] == '\0')
        return SPECIE_BAD_NAME;

    // We initialize the prev and current variables as the root node and NULL respectively.
    TNode *prev = &(trie->trie);
    TNode *current = NULL;

    // We loop through specieName's characters, in each step going down the correct branch
    // in the tree (creating nodes as necessary) until we've reached the end of the string.
    while (specieName[0] != '\0')
    {
        int currentCharIndex = getCharIndex(specieName[0]);

        current = prev->nextNodes[currentCharIndex];

        // If the node 'current' doesnt exist, we create it. If that fails, we return TRIE_NO_MEMORY.
        if (current == NULL && (current = addNode(prev, currentCharIndex)) == NULL)
            return SPECIE_NO_MEMORY;

        specieName++;
        prev = current;
    }

    // We increment current->count, to indicate one more ocurrance of this string.
    // At this point, we're ensured current is not NULL, because since the string
    // isn't null nor empty, we must have entered the while loop at least once.
    current->count++;
    return SPECIE_OK;
}

/* Recursively goes through all the nodes and subnodes, comparing and leaving
** the node with the highest node.count in the variable pointed to by mostPopular
** and said node's depth in the variable pointer to by mostDepth. */
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

    // We recursively search for the node with the highest node.count.
    findMostPopularRecursive(&(trie->trie), LOWEST_NODE_DEPTH, &mostPopular, &mostDepth);
    mostDepth -= LOWEST_NODE_DEPTH;

    if (mostPopular == NULL || mostPopular->count == 0)
        return NULL;

    char *str;
    if (!tryMalloc((void **)&str, mostDepth + 1))
        return NULL;

    // We backtrack from mostPopular all the way back to the tree's root, filling
    // up the string along the way.
    TNode *node = mostPopular;
    for (int i = 0; node->previousCharIndex >= 0; i++)
    {
        str[mostDepth - i - 1] = getIndexChar(node->previousCharIndex);
        node = node->previous;
    }

    // Since the trie doesn't keep track of casing, we uppercase the first character.
    str[0] = toupper(str[0]);

    str[mostDepth] = '\0';
    return str;
}