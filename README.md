# About the project

This project implements a csv reader to process the data from the trees of two cities: CABA, Argentina and Vancouver, Canada.

# Compile and run the project

This guide assumes you're either on Windows using mingw or on Arch linux.

## Prerequisites

* Make
* GCC

Then run the following commands to build this project:

1. Clone the repo.

```
git clone https://github.com/ThomasMiz/TPE-PI.git
cd TPE-PI
```

2. Compile the project.

```
make all
```

3. Run the project choosing one of the follows comands. 

```
./datasetBUE.out barriosBUE.csv arbolesBUE.csv
./datasetVAN.out barriosVAN.csv arbolesVAN.csv
```
