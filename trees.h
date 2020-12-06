#ifndef trees_h
#define trees_h

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

#endif