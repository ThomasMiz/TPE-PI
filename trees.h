#ifndef trees_h
#define trees_h

/* Defines errors that can be thrown by operations from trees processing. */
enum TREES_ERR
{
    TREES_OK = 0,
    TREES_NO_FILE,
    TREES_MISSING_COLUMN,
    TREES_NO_MEMORY,
    TREES_NO_DATA
};

/* Process all the trees into zone structure if no error has encountered,
** by reading them from a CSV file. */
enum TREES_ERR processTrees(const char *file);

/* Frees all the resources used by processTrees. */
void freeTrees();

#endif