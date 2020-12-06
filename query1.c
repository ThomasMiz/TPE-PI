#include <stdio.h>
#include <stdlib.h>
#include "query1.h"
#include "memhelper.h"
#include "zones.h"

#define QUERY1_FILE "query1.csv"
#define COL_NAME_ZONE "BARRIO"
#define COL_NAME_PERCENT "ARBOLES_POR_HABITANTE"

#define ROUND(x) ((int)((x)*100 + 0.5) / 100.0)

static enum QUERY1_ERR writeToCsv(TZone **allZones, float *percents, size_t dim)
{
    FILE *file = fopen(QUERY1_FILE, "w");

    if (file == NULL)
        return QUERY1_CANNOT_OPEN_FILE;

    fprintf(file, COL_NAME_ZONE ";" COL_NAME_PERCENT "\n");

    for (int i = 0; i < dim; i++)
        fprintf(file, "%s;%0.2f\n", allZones[i]->name, percents[i]);

    fclose(file);
    return QUERY1_OK;
}

static void sortTogether(float *vec, TZone **allZones, size_t dim)
{
    float tmp1;
    TZone *tmp2;
    for (int i = 0; i < dim; i++)
    {
        for (int j = i; j > 0 && vec[j - 1] < vec[j]; j--)
        {
            tmp1 = vec[j];
            vec[j] = vec[j - 1];
            vec[j - 1] = tmp1;

            tmp2 = allZones[j];
            allZones[j] = allZones[j - 1];
            allZones[j - 1] = tmp2;
        }
    }
}

enum QUERY1_ERR saveQuery1()
{
    size_t dim;
    TZone **allZones = getAllZones(&dim);

    if (allZones == NULL)
        return QUERY1_NO_MEMORY;

    float *vec;
    if (!tryMalloc((void **)&vec, dim * sizeof(float)))
    {
        free(allZones);
        return QUERY1_NO_MEMORY;
    }

    for (int i = 0; i < dim; i++)
        vec[i] = ROUND(allZones[i]->treeCount / (float)(allZones[i]->population));

    sortTogether(vec, allZones, dim);

    enum QUERY1_ERR writeResult = writeToCsv(allZones, vec, dim);

    free(allZones);
    free(vec);

    return writeResult;
}
