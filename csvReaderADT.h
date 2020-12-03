// CSV Reader ADT: Facilita la lectura de un archivo CSV.

// Para usarlo, se debe crear un csvReaderADT indicando con un string un path
// a el archivo que se desea abrir. Luego, se debe llamar a csvReadHeader(),
// indicando los nombres de los tokens que se desean leer.
// Ahora se puede comenzar a leer informacion del archivo. Con csvNextLine()
// podemos saltar a la siguiente linea (esto debe hacerse despues de leer el
// header). En cada linea, usamos csvNextToken() para leer el contenido de una
// de las columnas que se pidieron en csvReaderHeader() y asi obtener la data.

typedef struct csvReaderCDT *csvReaderADT;

#define CSV_ALREADY_INITIALIZED -1
#define CSV_NO_MEMORY -2

// Crea un nuevo csvReaderADT y lo inicializa para leer el archivo provisto.
// Si no se pudo abrir el archivo, no crea un reader y retorna NULL.
csvReaderADT newCsvReader(const char *file);

// Libera todos los recursos utilizados por el lector.
void freeCsvReader(csvReaderADT);

// Retorna 1 si el lector llego al final del archivo.
int csvEndOfFile(csvReaderADT);

// Para usar en la primera linea (el header).
// Busca para cada nombre provisto la columna en la que se encuentra, y guarda ese
// numero en el vector columnas. Si algun nombre no aparece, quedara en -1.
// Cuando se este usando csvNextToken, solo apareceran las columnas pedidas aca.
// Retorna la cantidad de columnas encontradas, o si hubo un error retornara
// CSV_ALREADY_INITIALIZED o CSV_NO_MEMORY
int csvReadHeader(csvReaderADT, const char *const names[], int columns[], size_t dim);

// Lee el siguiente token y retorna un string con el contenido y deja su longitud en strLen.
// El puntero retornado NO DEBE SER GUARDADO. Apunta a un buffer interno y sera modificado
// para cuando se lea el proximo token (esto se realiza por razones de performance).
// En nroColumna se dejara el numero de columna del dato que fue retornado.
const char *csvNextToken(csvReaderADT, size_t *strLen, int *nroColumna);

// Avanza el lector hasta el principio de la siguente linea, o hasta el final del archivo.
// Retorna 1 si quedan lineas para leer.
int csvNextLine(csvReaderADT);
