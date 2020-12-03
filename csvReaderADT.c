#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "memhelper.h"
#include "csvReaderADT.h"

// La capacidad maxima permitida para un buffer. Cualquier token mas largo que esta
// cantidad de chars sera cortado a esta cantidad.
// Esta sera la longitud del string, INCLUYENDO el '\0'.
#define MAX_BUFF_CAPACITY 129

struct csvReaderCDT
{
    // El archivo que se esta leyendo.
    FILE *file;

    // Un buffer utilizado para leer los datos.
    char *buff;
    size_t buff_count;

    // Un vector que contiene las columnas que debemos leer (ordenadas).
    // (los numeros de columnas que no esten en este vector seran salteados).
    size_t columnsCount;
    int *requiredColumns;

    // Cuando se este leyendo una linea, esto guarda el indice de columna actual
    // en el que estamos parados en la posicion actual de lectura del archivo.
    int currentColumn;

    // Cuando se este leyendo una linea, este es el indice en requiredColumns
    // de la siguiente columna que se debe leer en el siguiente csvNextToken().
    int requiredColumnIndex;
};

csvReaderADT newCsvReader(const char *file)
{
    // Intentamos abrir el archivo provisto.
    FILE *f = fopen(file, "rt");

    // Si no pudimos abrir el archivo, no creamos un lector. Retornamos NULL.
    if (f == NULL)
        return NULL;

    // Creamos y retornamos un nuevo lector.
    csvReaderADT r;
    if (!tryCalloc((void **)&r, 1, sizeof(struct csvReaderCDT)))
        return NULL;

    // Intentamos alocar un buffer para la lectura de tokens. Si falla, retornamos NULL.
    if (!tryMalloc((void **)&(r->buff), MAX_BUFF_CAPACITY))
    {
        free(r);
        return NULL;
    }

    r->requiredColumns = NULL;
    r->file = f;
    return r;
}

void freeCsvReader(csvReaderADT reader)
{
    // Cerramos el archivo.
    fclose(reader->file);
    reader->file = NULL;

    // Liberamos los vectores guardados por la estructura.
    free(reader->requiredColumns);
    free(reader->buff);

    // Liberamos la estructura misma.
    free(reader);
}

int csvEndOfFile(csvReaderADT reader)
{
    return feof(reader->file);
}

// Retorna 1 si el lector del archivo esta parado al final de una linea
// (esto incluye el final del archivo). No se avanza la posicion del lector.
static int isEndOfLine(FILE *file)
{
    char c = fgetc(file);
    if (c == EOF)
        return 1;

    int ret = c == '\n';
    ungetc(c, file);
    return ret;
}

// Lee un token del archivo, avanzando el lector y guardando el contenido
// en el buffer interno del csvReader.
static void readTokenToBuff(csvReaderADT reader)
{
    FILE *file = reader->file;
    reader->buff_count = 0;
    char c;

    // Avanzamos un char y lo guardamos en el buffer, hasta llegar al final del token.
    while ((c = fgetc(file)) != EOF && c != '\n' && c != ';')
    {
        // Para asegurarnos de ignorar caracteres de control como '\r'.
        if (c >= 32 && reader->buff_count < MAX_BUFF_CAPACITY - 1)
        {
            // Agregamos el caracter al buffer y aumentamos el contador del buffer.
            reader->buff[reader->buff_count++] = c;
        }
    }

    // Ponemos un '\0' despues del ultimo caracter en el buffer.
    reader->buff[reader->buff_count] = '\0';

    // Si el ultimo char que lemos del archivo es un '\n', lo un-get-eamos, tal que
    // no se avance de linea hasta que se llame csvNextLine().
    if (c == '\n')
        ungetc('\n', file);
}

int csvReadHeader(csvReaderADT reader, const char *const names[], int columns[], size_t dim)
{
    // Si ya se leyo el header, se esta usando mal el TAD...
    if (reader->requiredColumns != NULL)
        return CSV_ALREADY_INITIALIZED;

    reader->columnsCount = 0;
    if (!tryMalloc((void **)&(reader->requiredColumns), sizeof(int) * dim))
        return CSV_NO_MEMORY;

    // Ponemos todo el vector de columns en -1. Cualquiera que no toquemos queda en -1.
    for (int i = 0; i < dim; i++)
        columns[i] = -1;
    size_t count = 0;

    FILE *file = reader->file;

    int columnNumber = 0;
    while (!isEndOfLine(file) && count < dim)
    {
        readTokenToBuff(reader);

        for (int i = 0; i < dim && count < dim; i++)
        {
            if (columns[i] == -1 && strcmp(reader->buff, names[i]) == 0)
            {
                columns[i] = columnNumber;
                reader->requiredColumns[reader->columnsCount++] = columnNumber;
                count++;
            }
        }

        columnNumber++;
    }

    if (reader->columnsCount != dim)
        reader->requiredColumns = realloc(reader->requiredColumns, sizeof(int) * reader->columnsCount);

    return count;
}

// Lee un token del archivo, avanzando el lector pero NO GUARDANDO el contenido
// en el buffer interno del csvReader.
// Retorna 1 si hay mas columnas que leer en la linea actual.
static int skipColumn(FILE *file)
{
    // Leemos un char hasta llegar al final del token/linea/archivo.
    char c;
    do
    {
        c = fgetc(file);
    } while (c != EOF && c != '\n' && c != ';');

    // Si llegamos al final de una linea, devolvemos el '\n' tal de no avanzar de linea
    // hasta que se haya llamado csvNextLine().
    if (c == '\n')
        ungetc('\n', file);

    return c != '\n' && c != EOF;
}

const char *csvNextToken(csvReaderADT reader, size_t *strLen, int *nroColumna)
{
    *strLen = 0;
    *nroColumna = -1;
    if (reader->requiredColumnIndex == reader->columnsCount)
        return NULL;

    FILE *file = reader->file;

    int nextColumn = reader->requiredColumns[reader->requiredColumnIndex++];
    while (reader->currentColumn < nextColumn)
    {
        if (!skipColumn(file))
            return NULL;
        reader->currentColumn++;
    }

    readTokenToBuff(reader);
    reader->currentColumn++;

    *nroColumna = nextColumn;
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

    // Si encontramos una nueva linea, nos aseguramos que no sea una linea vacia!
    if (c == '\n')
    {
        // Leemos un char. Si es el final de linea, es porque el archivo termina con
        // una linea vacia. Tomamos entonces como que llegamos al final del archivo.
        // Si no, devolvemos el char que sacamos para que pueda ser leido proximamente.
        c = fgetc(file);
        if (c != EOF)
            ungetc(c, file);
    }

    // Reseteamos las variables que trackean columnas.
    reader->currentColumn = 0;
    reader->requiredColumnIndex = 0;
    return c != EOF;
}
