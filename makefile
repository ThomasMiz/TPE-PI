COMPILER = gcc
OUTPUT_FILE_BUE = datasetBUE.out
OUTPUT_FILE_VAN = datasetVAN.out
FILES = main.c csvReaderADT.c memhelper.c query1.c query2.c specieCounterADT.c trees.c zones.c
FLAGS = -std=c99 -pedantic -Wall -fsanitize=address
ARG_BUE = barriosBUE.csv arbolesBUE.csv
ARG_VAN = barriosVAN.csv arbolesVAN.csv

all: 
	datasetBUE datasetVAN

datasetBUE:
	$(COMPILER) -o $(OUTPUT_FILE_BUE) $(FILES) $(FLAGS) -D TREE_ZONENAME=3 -D TREE_SPECIESNAME=8 -D ZONE_ZONENAME=1 -D ZONE_POPULATION=2
	
datasetVAN:
	$(COMPILER) -o $(OUTPUT_FILE_VAN) $(FILES) $(FLAGS) -D TREE_ZONENAME=13 -D TREE_SPECIESNAME=7 -D ZONE_ZONENAME=1 -D ZONE_POPULATION=2

clean:
	rm -r $(OUTPUT_FILE_BUE) $(OUTPUT_FILE_VAN) *.o