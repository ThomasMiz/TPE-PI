#include <stdio.h>
#include <stdlib.h>
#include "query1.h"
#include "zones.h"
#define QUERY1_FILE "query1.csv"
#define COL_NAME_ZONE "BARRIO"
#define COL_NAME_PERCENT "ARBOLES_POR_HABITANTE"

static enum QUERY1_ERR writeToCsv(TZone ** allZones, float * percents, size_t dim) {

    FILE* file = fopen(QUERY1_FILE, "w");

    if(file == NULL)
        return CANNOT_OPEN_FILE;

    fprintf(file, COL_NAME_ZONE";"COL_NAME_PERCENT"\n");

    for(int i = 0 ; i < dim ; i++){

        fprintf(file, "%s;%0.2f\n", allZones[i]->name, percents[i]);
    }

    fclose(file);
    return QUERY1_OK;
}

enum QUERY1_ERR calcTreesPerPop() {

    size_t dim;
    TZone ** allZones = getAllZones(&dim);

    if(allZones == NULL)
        return QUERY1_NO_MEMORY;

    float vec[dim];

    for(int i = 0 ; i < dim ; i++){
        vec[i] = allZones[i]->treeCount / (float)(allZones[i]->population); 
    }

    int ordered = 0;
    int j = 0;
    while(ordered == 0) {
        ordered = 1;
        for(int i = 0 ; i < dim - i ; i++){
            if (vec[i] > vec[i+1]){

                int aux = vec[i];
                vec[i] = vec[i+1];
                vec[i+1] = aux;

                TZone * aux2 = allZones[i];
                allZones[i] = allZones[i+1];
                allZones[i+1] = aux2;

                ordered = 0;
            }
        }
        j++;
    }

    writeToCsv(allZones, vec, dim);

    free(allZones);

    return QUERY1_OK;
    
}