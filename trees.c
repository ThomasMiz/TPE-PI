#include <stdlib.h>
#include <string.h>
#include "memhelper.h"
#include "csvReaderADT.h"
#include "specieCounterADT.h"
#include "zones.h"
#include "trees.h"
#include "defines.h"

static enum TREES_ERR readTree(csvReaderADT reader, int keyCount, char *buffSpecie)
{
    const char *tokenStr;
    size_t len, specieLen = 0;
    int tokenCol;

    TZone *zone = NULL;
    buffSpecie[0] = '\0';

    for (int i = 0; i < keyCount; i++)
    {
        tokenStr = csvNextToken(reader, &len, &tokenCol);

        // In case new columns are added they would be added as new cases.
        switch (tokenCol)
        {
        case TREE_ZONENAME:
            if (len != 0)
                zone = getZoneByName(tokenStr);
            break;

        case TREE_SPECIESNAME:
            // If at any time a file interprets unknown species as an empty string remains buffSpecie[0]='\0'.
            // We only need to save the name of the species because
            // we don't know if it appears before the name of the zone.
            specieLen = len;
            if (len != 0)
                strcpy(buffSpecie, tokenStr);
            break;

        default:
            break;
        }
    }

    // If the zone exists then we add a new tree and add a new species if it didn't exist before.
    if (zone != NULL)
    {
        zone->treeCount++;
        enum SPECIE_ERR specieResult = specieAddOne(zone->species, buffSpecie, specieLen);
        if (specieResult == SPECIE_NO_MEMORY)
            return TREES_NO_MEMORY;
    }

    return TREES_OK;
}

// Static functions to add the speciesCounterADT in each zone.

static int setupSpecies(TZone *zone)
{
    zone->species = newSpecieCounter();
    zone->treeCount = 0;
    return 1;
}

// Static functions to free the speciesCounterADT in each zone.
static int unwrapSpecies(TZone *zone)
{
    if (zone->species != NULL)
        freeSpecieCounter(zone->species);
    return 1;
}

void freeTrees()
{
    zonesForEach(unwrapSpecies);
}

enum TREES_ERR processTrees(const char *file)
{
    // We create the reader and ensure it was created successfully.
    csvReaderADT reader = newCsvReader(file);
    if (reader == NULL)
        return TREES_NO_FILE;

    // Columns of interest in the file, if more needed they would be added to the vector.
    int columns[] = {TREE_ZONENAME, TREE_SPECIESNAME};
    int keyCount = sizeof(columns) / sizeof(columns[0]);
    enum CSV_ERR headerResult = csvSetupHeader(reader, columns, keyCount);

    if (headerResult != CSV_OK)
    {
        freeCsvReader(reader);
        if (headerResult == CSV_NO_MEMORY)
            return TREES_NO_MEMORY;
        return TREES_MISSING_COLUMN;
    }

    // Buffer to save species names in case they appear before the zone.
    char *buffSpecie;
    if (!tryMalloc((void **)&buffSpecie, MAX_BUFF_CAPACITY))
    {
        freeCsvReader(reader);
        return TREES_NO_MEMORY;
    }

    zonesForEach(setupSpecies);

    // We read trees data.
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

    freeCsvReader(reader);
    free(buffSpecie);
    return TREES_OK;
}
