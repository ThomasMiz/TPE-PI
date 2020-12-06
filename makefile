COMPILER = gcc
OUTPUT_FILE_BUE = datasetBUE.out
OUTPUT_FILE_VAN = datasetVAN.out
FILES = main.c csvReaderADT.c memhelper.c query1.c query2.c specieCounterADT.c trees.c zones.c
FLAGS = -std=c99 -pedantic -Wall -fsanitize=address

all: datasetBUE datasetVAN

datasetBUE:
	$(COMPILER) -o $(OUTPUT_FILE_BUE) $(FILES) $(FLAGS) -D BUE
	
datasetVAN:
	$(COMPILER) -o $(OUTPUT_FILE_VAN) $(FILES) $(FLAGS) -D VAN

clean:
	rm -r *.out
