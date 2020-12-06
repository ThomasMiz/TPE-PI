#ifndef zones_h
#define zones_h
#include "specieCounterADT.h"

/* When the population of a zone isn't present, this value will be asigned. */
#define NUMBER_NOT_PRESENT 0

/* Represents a zone. For example, a neighborhood. */
typedef struct
{
    const char *name;
    unsigned long population;
    unsigned long treeCount;
    specieCounterADT species;
} TZone;

/* Defines errors that can be thrown by operations from zones processing. */
enum ZONES_ERR
{
    ZONES_OK = 0,
    ZONES_ALREADY_INITIALIZED,
    ZONES_NO_FILE,
    ZONES_MISSING_COLUMN,
    ZONES_NO_MEMORY,
    ZONES_REPEATED
};

/* Loads all the zones into memory if no error has encountered,
** by reading them from a CSV file. */
enum ZONES_ERR initializeZones(const char *file);

/* Frees all the resources used by zones. This includes the name strings
** all zones have. */
void freeZones();

/* Looks for a zone by name and returns that zone. If no zone is found, returns NULL. */
TZone *getZoneByName(const char *name);

/* Executes the provided function for each loaded zone, in alphabetical order.
** If returns 1 continues and if not, it cuts the execution. */
void zonesForEach(int (*func)(TZone *));

/* Returns an array with all zones and NULL if there are no zones or if it ran out of memory. */
TZone **getAllZones(size_t *dim);

#endif
