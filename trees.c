#include "csvReaderADT.h"
#include "memhelper.h"
#include "specieTrieADT.h"
#include "zones.h"
#include "trees.h"
#include <stdlib.h>
#include <string.h>

#define COL_ZONENAME 3
#define COL_SPECIENAME 8

static enum TREES_ERR readTree(csvReaderADT reader, int keyCount, char *buffSpecie)
{
    const char *tokenStr;
    size_t len;
    int tokenCol;
    TZone *zone;

    for (int i = 0; i < keyCount; i++)
    {
        tokenStr = csvNextToken(reader, &len, &tokenCol);

        switch (tokenCol)
        {
        case COL_ZONENAME:
            zone = getZoneByName(tokenStr);
            if (zone != NULL)
                zone->treeCount++;
            break;

        case COL_SPECIENAME:
            // If at any time a file interprets unknown species as an empty string.
            if (tokenStr == NULL || len == 0)
                buffSpecie[0] = '\0';
            else
                // We only need to save the name of the species because
                // we don't know if it appears before the name of the neighborhood.
                strcpy(buffSpecie, tokenStr);
            break;

        default:
            break;
        }
    }

    enum SPECIE_ERR specieResult = specieAddOne(zone->species, buffSpecie);

    if (specieResult != SPECIE_OK)
        return TREES_NO_DATA;

    return TREES_OK;
}

static int setupSpecies(TZone *zone)
{
    zone->species = newSpecieTrie();
    zone->treeCount = 0;
    return 1;
}

static int unwrapSpecies(TZone *zone)
{
    freeSpecieTrie(zone->species);
    return 1;
}

enum TREES_ERR processTrees(const char *file)
{
    zonesForEach(setupSpecies);
    // We create the reader and ensure it was created successfully.
    csvReaderADT reader = newCsvReader(file);
    if (reader == NULL)
        return TREES_NO_FILE;

    int columns[] = {COL_ZONENAME, COL_SPECIENAME};
    int keyCount = sizeof(columns) / sizeof(columns[0]);
    int headerResult = csvSetupHeader(reader, columns, keyCount);

    if (headerResult < 0)
    {
        freeCsvReader(reader);
        if (headerResult == CSV_NO_MEMORY)
            return TREES_NO_MEMORY;

        return TREES_MISSING_COLUMN;
    }

    char *buffSpecie;
    if (!tryMalloc((void **)&buffSpecie, MAX_BUFF_CAPACITY))
        return TREES_NO_MEMORY;

    while (csvNextLine(reader))
    {
        enum TREES_ERR treeResult = readTree(reader, keyCount, buffSpecie);
        // If there is an error reading a tree, the process ends without reading
        // the trees that follow, is a contract matter.
        if (treeResult != TREES_OK)
        {
            freeCsvReader(reader);
            free(buffSpecie);
            return treeResult;
        }
    }

    zonesForEach(unwrapSpecies);
    freeCsvReader(reader);
    free(buffSpecie);
    return TREES_OK;
}