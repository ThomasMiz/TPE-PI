#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "memhelper.h"
#include "csvReaderADT.h"

// The maximum capacity allowed for a buffer. Any token longer than this amount of chars will be trimmed to this amount.
// This will be the length of the string, Including '\ 0.
#define MAX_BUFF_CAPACITY 129

struct csvReaderCDT{
    // The file that is of interest to the reader.
    FILE *file;

    // A buffer used to read the data.
    char *buff;
    size_t buff_count;

    // A vector containing the columns to be read, in order.
    // Column numbers that are not in this vector will be skipped.
    size_t columnsCount;
    int *requiredColumns;

    // When a line is being read, saves the column number the reader is standing on.
    int currentColumn;

    // When a line is being read, saves the column number of the next column to be read.
    int requiredColumnIndex;
};

csvReaderADT newCsvReader(const char *file)
{
    // Try to open the file.
    FILE *f = fopen(file, "rt");

    // If the file cannot be opened, the reader is not created and returns NULL.
    if (f == NULL)
        return NULL;

    // A reader is created and returned.
    csvReaderADT r;

    if (!tryCalloc((void **)&r, 1, sizeof(struct csvReaderCDT)))
        return NULL;

    if (!tryMalloc((void **)&(r->buff), MAX_BUFF_CAPACITY))
    {
        free(r);
        return NULL;
    }

    r->file = f;
    return r;
}

void freeCsvReader(csvReaderADT reader){
    // The file is closed.
    fclose(reader->file);
    reader->file = NULL;

    // The vectors stored by the structure are freed.
    free(reader->requiredColumns);
    free(reader->buff);

    // The struct is freed.
    free(reader);
}

int csvEndOfFile(csvReaderADT reader){
    return feof(reader->file);
}

// Returns 1 if the file reader is at the end of a line (this includes EOF). 
// The position of the reader does not advance.
static int isEndOfLine(FILE *file)
{
    char c = fgetc(file);
    if (c == EOF)
        return 1;

    int ret = c == '\n';
    ungetc(c, file);
    return ret;
}

// Read a token from the file, advancing the reader and saving the content
// in the internal buffer of the csvReader.
// In this way we save the fgets plus strtok call, 
static void readTokenToBuff(csvReaderADT reader)
{
    FILE *file = reader->file;
    reader->buff_count = 0;
    char c;

    // We move one position (a char) and save it in the buffer, until we reach the end of the token.
    while ((c = fgetc(file)) != EOF && c != '\n' && c != ';')
    {
        // To make sure to ignore control characters like '\r'.
        if (c >= 32 && reader->buff_count < MAX_BUFF_CAPACITY - 1)
        {
            // We add the character to the buffer and increase the buffer counter.
            reader->buff[reader->buff_count++] = c;
        }
    }

    reader->buff[reader->buff_count] = '\0';

    // If the char we read is a '\n', we ungetc-it, so that no line is advanced until csvNextLine () is called.
    if (c == '\n')
        ungetc('\n', file);
}

int csvReadHeader(csvReaderADT reader, int columns[], size_t dim)
{
    // Defensive programming in case the header is read more than once.
    if (reader->requiredColumns != NULL)
        return CSV_ALREADY_INITIALIZED;

    reader->columnsCount = 0;
    if (!tryMalloc((void **)&(reader->requiredColumns), sizeof(int) * dim))
        return CSV_NO_MEMORY;

    FILE *file = reader->file;
    
    size_t numberOfColums = 0;

    // The header is read.
    while (!isEndOfLine(file))
    {
        skipColumn(file);
        numberOfColums++;
    }

    // If a column that does not exist is needed, returns error.
    if(numberOfColums < columns[dim])
        return CSV_MAX_COLUMNS_ERROR;

    size_t count = 0;

    //The column of interest's numbers are saved in the structure.
    while(count<dim)
        reader->requiredColumns[reader->columnsCount++] = columns[count++];

    return count;
}

// Read a token from the file, the reader advances one column 
// but the content is not saved in the internal buffer of the csvReader.
// Returns 1 if there are more columns to read on the current line.
static int skipColumn(FILE *file)
{
    // The file is read until the end of the token/line/file is reached.
    char c;
    do
    {
        c = fgetc(file);
    } while (c != EOF && c != '\n' && c != ';');

    // If the char we read is a '\n', we ungetc-it, so that no line is advanced until csvNextLine () is called.
    if (c == '\n')
        ungetc('\n', file);

    return c != '\n' && c != EOF;
}

const char *csvNextToken(csvReaderADT reader, size_t *strLen, int *numCol)
{
    *strLen = 0;
    *numCol = -1;
    
    //If there are no more columns of interest on the line.
    if (reader->requiredColumnIndex == reader->columnsCount)
        return NULL;

    FILE *file = reader->file;

    int nextColumn = reader->requiredColumns[reader->requiredColumnIndex++];
    
    // As long as the reader is not positioned in a column of interest, columns are skipped without saving anything in memory.
    while (reader->currentColumn < nextColumn)
    {
        if (!skipColumn(file))
            return NULL;
        reader->currentColumn++;
    }

    readTokenToBuff(reader);
    reader->currentColumn++;

    *numCol = nextColumn;
    *strLen = reader->buff_count;
    return reader->buff;
}

int csvNextLine(csvReaderADT reader)
{
    FILE *file = reader->file;
    char c;
    do
    {
        c = fgetc(file);
    } while (c != EOF && c != '\n');

    // If a new line is found we make sure it is not an empty line.
    if (c == '\n')
    {
        // We read a char. If it is the end of the line, it is because the file ends with an empty line. 
        // We then assume that we have reached the end of the file. 
        // If not, we return the char we removed so that it can be read again.
        c = fgetc(file);
        if (c != EOF)
            ungetc(c, file);
    }

    // Variables that track columns are reset.
    reader->currentColumn = 0;
    reader->requiredColumnIndex = 0;
    return c != EOF;
}
