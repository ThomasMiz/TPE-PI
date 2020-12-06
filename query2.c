#include <stdio.h>
#include <stdlib.h>
#include "query2.h"
#include "zones.h"
#include "specieCounterADT.h"

#define QUERY2_FILE "query2.csv"
#define COL_NAME_ZONE "BARRIO"
#define COL_NAME_SPECIE_NAME "NOMBRE_CIENTIFICO"

static FILE *file;

static int printZone(TZone * zone)
{
    fprintf(file,"%s;%s\n",zone->name, specieMostPopular(zone->species));

    return 1;
}

enum QUERY2_ERR saveQuery2()
{
    file = fopen(QUERY2_FILE, "w");

    if (file == NULL)
        return QUERY2_CANNOT_OPEN_FILE;
    
    fprintf(file, COL_NAME_ZONE ";" COL_NAME_SPECIE_NAME "\n");
    zonesForEach(printZone);

    fclose(file);

    return QUERY2_OK;
}