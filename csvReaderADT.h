#ifndef csvReaderADT_h
#define csvReaderADT_h

/* CSV Reader ADT: for ease of reading CSV files. */

/*
** To use it, a csvReaderADT must be created indicating with a string the path of the file to be opened. Then, csvReadHeader () must be called, 
** indicating the numbers of columns of interest. Now it is possible to start reading information from the file. 
** With csvNextLine () we can jump to the next line (this must be done after reading the header). In each line, 
** we use csvNextToken () to read the content of one of the columns that were requested in csvReaderHeader () and thus obtain the data. */

typedef struct csvReaderCDT *csvReaderADT;

#define CSV_ALREADY_INITIALIZED -1
#define CSV_NO_MEMORY -2
#define CSV_MAX_COLUMNS_ERROR -3

/* Create a new csvReaderADT and initialize it to read the provided file. 
** If the file could not be opened, it does not create a reader and returns NULL. */

csvReaderADT newCsvReader(const char *file);

/* All resources used by the reader are . */
void freeCsvReader(csvReaderADT);

/* Returns 1 if the reader reached the end of the file. */
int csvEndOfFile(csvReaderADT);

/* To use in the first line, the header.
** Save the column of interest's numbers in the structure from the vector colums, previously ordered. If there are not so many columns, it will return CSV_MAX_COLUMNS_ERROR.
** Returns the number of columns found, or if there was an error it will return CSV_ALREADY_INITIALIZED or CSV_NO_MEMORY. */
int csvReadHeader(csvReaderADT, int columns[], size_t dim);

/* Read the next token and return a string with the content and leave its length in strLen.
** The returned pointer MUST NOT BE SAVED, it points to an internal buffer and it will be modified
** when the next token is read (this is done for performance reasons).
** In numCol the column number of the returned data will be saved */
const char *csvNextToken(csvReaderADT, size_t *strLen, int *numCol);

/* Advance the reader to the beginning of the next line, or to the end of the file.
** Returns 1 if there are left lines to read. */
int csvNextLine(csvReaderADT);

#endif