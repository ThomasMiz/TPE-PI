#include "query1.h"
#include "query2.h"
#include "csvReaderADT.h"
#include "specieCounterADT.h"
#include "zones.h"
#include "trees.h"
#include <stdio.h>
#include <stdlib.h>

#define OK 0
#define ARG_EXP 3
#define ARG_ERR 1
#define ARC_ZONES 1
#define ARC_TREES 2
#define ARC_ERR 2
#define MEMORY_ERR "A memory error has encountered.\n"

int main(int argc, char *argv[])
{
    printf("%d\n",argc);
    for(int i=0; i<argc; i++){
        printf("%d: %s\n",i,argv[i]);
    }

    if (argc > ARG_EXP)
    {
        printf("Too many arguments supplied.\n");
        return ARG_ERR;
    }
    if (argc < ARG_EXP)
    {
        printf("Not enough arguments supplied.\n");
        return ARG_ERR;
    }

    enum ZONES_ERR zoneResult = initializeZones(argv[ARC_ZONES]);

    if (zoneResult != ZONES_OK)
    {
        switch (zoneResult)
        {
        case ZONES_NO_FILE:
            printf("Zone file not found or doesn't exist.\n");
            break;
        case ZONES_NO_MEMORY:
            printf(MEMORY_ERR);
            break;
        case ZONES_MISSING_COLUMN:
            printf("A column was not found in the zone file.\n");
            break;
        default:
            printf("Unspecified error.\n");
            break;
        }
        freeZones();
        return ARC_ERR;
    }

    enum TREES_ERR treeResult = processTrees(argv[ARC_TREES]);

    if (treeResult != TREES_OK)
    {
        switch (treeResult)
        {
        case TREES_NO_FILE:
            printf("Tree file not found or doesn't exist.\n");
            break;
        case TREES_NO_MEMORY:
            printf(MEMORY_ERR);
            break;
        case TREES_MISSING_COLUMN:
            printf("A column was not found in the tree file.\n");
            break;
        default:
            printf("Unspecified error.\n");
            break;
        }
        freeTrees();
        freeZones();
        return ARC_ERR;
    }

    enum QUERY1_ERR query1Result = saveQuery1();

    if(query1Result != QUERY1_OK)
    {
        switch (query1Result)
        {
        case QUERY1_CANNOT_OPEN_FILE:
            printf("Query1 cannot write file.\n");
            break;
        case QUERY1_NO_MEMORY:
            printf(MEMORY_ERR);
            break;
        default:
            printf("Unspecified error.\n");
            break;
        }
        freeTrees();
        freeZones();
        return ARC_ERR;
    }

    enum QUERY2_ERR query2Result = saveQuery2();

    if(query2Result != QUERY2_OK)
    {
        switch (query2Result)
        {
        case QUERY2_CANNOT_OPEN_FILE:
            printf("Query2 cannot write file.\n");
            break;
        default:
            printf("Unspecified error.\n");
            break;
        }
        freeTrees();
        freeZones();
        return ARC_ERR;
    }

    freeTrees();
    freeZones();

    return OK;
}