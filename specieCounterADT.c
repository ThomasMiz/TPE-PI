#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "specieCounterADT.h"
#include "memhelper.h"

/* The lowest depth value for a node. Used for calculations. */
#define LOWEST_NODE_DEPTH 0

typedef struct listNode
{
    char *name;
    unsigned int count;
    struct listNode *next;
} TNode;

typedef TNode *TList;

struct specieCounterCDT
{
    TList list;
};

specieCounterADT newSpecieCounter()
{
    specieCounterADT species;
    if (!tryMalloc((void **)&species, sizeof(struct specieCounterCDT)))
        return NULL;
    species->list = NULL;
    return species;
}

void freeSpecieCounter(specieCounterADT species)
{
    TNode *next, *node = species->list;
    while (node != NULL)
    {
        next = node->next;
        free(node->name);
        free(node);
        node = next;
    }

    free(species);
}

enum SPECIE_ERR specieAddOne(specieCounterADT species, const char *specieName, size_t nameLen)
{
    // We first check that we received a non-null-or-empty string.
    if (specieName == NULL || specieName[0] == '\0')
        return SPECIE_BAD_NAME;

    int c;
    if (species->list == NULL || (c = strcasecmp(specieName, species->list->name) < 0))
    {
        TNode *newNode;
        if (!tryMalloc((void **)&newNode, sizeof(TNode)))
            return SPECIE_NO_MEMORY;
        newNode->count = 1;
        newNode->next = species->list;
        species->list = newNode;
        if (!tryMalloc((void **)&(newNode->name), nameLen + 1))
        {
            newNode->name = NULL;
            return SPECIE_NO_MEMORY;
        }
        strcpy(newNode->name, specieName);
    }

    if (c == 0)
    {
        species->list->count++;
        return SPECIE_OK;
    }

    TNode *prev = species->list;
    TNode *current = prev->next;

    while (current != NULL && (c = strcasecmp(specieName, current->name)) > 0)
    {
        prev = current;
        current = current->next;
    }

    if (c == 0)
    {
        current->count++;
        return SPECIE_OK;
    }

    TNode *newNode;
    if (!tryMalloc((void **)&newNode, sizeof(TNode)))
        return SPECIE_NO_MEMORY;
    newNode->count = 1;
    newNode->next = current;
    prev->next = newNode;
    if (!tryMalloc((void **)&(newNode->name), nameLen + 1))
    {
        newNode->name = NULL;
        return SPECIE_NO_MEMORY;
    }
    strcpy(newNode->name, specieName);

    return SPECIE_OK;
}

const char *specieMostPopular(specieCounterADT species)
{
    if (species->list == NULL)
        return NULL;

    TNode *mostPopular = species->list;
    TNode *current = mostPopular->next;

    while (current != NULL)
    {
        if (current->count > mostPopular->count)
            mostPopular = current;
        current = current->next;
    }

    return mostPopular->name;
}
