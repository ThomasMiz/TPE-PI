#ifndef query1_h
#define query1_h

enum QUERY1_ERR
{
    QUERY1_OK = 0,
    QUERY1_CANNOT_OPEN_FILE,
    QUERY1_NO_MEMORY

};

/* Calculates the information required by the query1 and saves it to a new csv file. */
enum QUERY1_ERR saveQuery1();

#endif
