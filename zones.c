#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "csvReaderADT.h"
#include "memhelper.h"
#include "zones.h"

// The identifier in the csv header for the column that contains a zone's name.
#define COL_ZONENAME 1
// The identifier in the csv header for the column that contains a zone's population.
#define COL_POPULATION 2

typedef struct node
{
    TZone zone;
    struct node *next;
} TNode;

typedef TNode *TList;

static TList zones = NULL;
static size_t zoneCount = 0;

// Adds a zone to the structure if no error has encountered.
static enum ZONES_ERR addZone(TZone newZone)
{
    // We created a new node at the beginning of the function since the existence of two or more zones
    // with the same name in the file is a uncommon error and is not considered for this optimization.
    TList newNode;
    if (!tryMalloc((void **)&newNode, sizeof(TNode)))
        return ZONES_NO_MEMORY;

    newNode->zone = newZone;

    int c;
    // We check if the neighborhood to be added has to go first on the list.
    if (zones == NULL || (c = strcasecmp(newZone.name, zones->zone.name)) < 0)
    {
        newNode->next = zones;
        zones = newNode;
        zoneCount++;
        return ZONES_OK;
    }

    TList previousZone = zones;
    TList currentZone = zones->next;

    // We move through the list until it ends, finds a repeating zone or needs to add a zone.
    while (currentZone != NULL && (c = strcasecmp(newZone.name, currentZone->zone.name)) > 0)
    {
        previousZone = currentZone;
        currentZone = currentZone->next;
    }

    if (c == 0)
    {
        free(newNode);
        return ZONES_REPEATED;
    }

    newNode->next = currentZone;
    previousZone->next = newNode;
    zoneCount++;

    return ZONES_OK;
}

// Reads a zone to the structure if no error has encountered.
static enum ZONES_ERR readZone(TZone *zone, csvReaderADT reader, int keyCount)
{
    zone->name = NULL;
    zone->population = NUMBER_NOT_PRESENT;

    const char *tokenStr;
    size_t len;
    int tokenCol;

    // We read the tokens from the current line and save them on tokenStr.
    for (int i = 0; i < keyCount; i++)
    {
        tokenStr = csvNextToken(reader, &len, &tokenCol);

        // In case new columns are added they would be added as new cases.
        switch (tokenCol)
        {
        case COL_ZONENAME:
            if (len != 0)
            {
                char *name;
                if (!tryMalloc((void **)&name, len + 1))
                    return ZONES_NO_MEMORY;

                strcpy(name, tokenStr);
                zone->name = name;
            }
            break;

        case COL_POPULATION:
            zone->population = (tokenStr == NULL || len == 0) ? NUMBER_NOT_PRESENT : atoi(tokenStr);
            break;

        default:
            break;
        }
    }

    return ZONES_OK;
}

enum ZONES_ERR initializeZones(const char *file)
{
    // Zones should only be initialized once, unique static structure.
    if (zones != NULL)
        return ZONES_ALREADY_INITIALIZED;

    // We create the reader and ensure it was created successfully.
    csvReaderADT reader = newCsvReader(file);
    if (reader == NULL)
        return ZONES_NO_FILE;

    // Columns of interest in the file, if more needed they would be added to the vector.
    int columns[] = {COL_ZONENAME, COL_POPULATION};
    int keyCount = sizeof(columns) / sizeof(columns[0]);
    enum CSV_ERR headerResult = csvSetupHeader(reader, columns, keyCount);

    if (headerResult != CSV_OK)
    {
        freeCsvReader(reader);
        if (headerResult == CSV_NO_MEMORY)
            return ZONES_NO_MEMORY;

        return ZONES_MISSING_COLUMN;
    }

    TZone zone;

    // We read zones data.
    while (csvNextLine(reader))
    {
        enum ZONES_ERR zoneResult = readZone(&zone, reader, keyCount);

        if (zoneResult == ZONES_OK)
            zoneResult = addZone(zone);

        if (zoneResult != ZONES_OK)
        {
            freeCsvReader(reader);

            if (zone.name != NULL)
                free((char *)zone.name);

            return zoneResult;
        }
    }

    freeCsvReader(reader);
    return ZONES_OK;
}

void freeZones()
{
    TList current = zones;

    while (current != NULL)
    {

        TList next = current->next;
        free((char *)current->zone.name);
        free(current);
        current = next;
    }
}

TZone *getZoneByName(const char *name)
{
    int c;
    TList current = zones;

    while (current != NULL && (c = strcasecmp(name, current->zone.name) > 0))
        current = current->next;

    return (current != NULL && c == 0) ? &(current->zone) : NULL;
}

void zonesForEach(int (*func)(TZone *))
{
    TList current = zones;
    while (current != NULL)
    {

        if (func(&(current->zone)))
            current = current->next;
        else
            return;
    }
}

TZone **getAllZones(size_t *dim)
{

    if (zones == NULL)
        return NULL;

    TZone **arrayZone;
    if (!tryMalloc((void **)&arrayZone, sizeof(TZone *) * zoneCount))
    {
        return NULL;
    }

    TList current = zones;
    int i = 0;
    while (current != NULL)
    {
        arrayZone[i++] = &(current->zone);
        current = current->next;
    }

    *dim = i;
    return arrayZone;
}
