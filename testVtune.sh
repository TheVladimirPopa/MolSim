#!/bin/bash

threadNums=(1 2 4 8 14 16 28 56)

# where the vtune executable is
vtune_loc=$1

# the directory where MolSim is located
working_dir=$2

# absolute path where the input xml is
inputXML=$3

# empty directory where the things should be written
outputDir=$4



for threadCount in "${threadNums[@]}";do
  cd "$outputDir"||return
  mkdir "thread$threadCount"
  OMP_NUM_THREADS="$threadCount" "$vtune_loc" -collect threading --app-working-dir="$working_dir" -r "$outputDir/thread$threadCount" ./MolSim -i "$inputXML" -p
done
