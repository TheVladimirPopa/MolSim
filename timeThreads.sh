#!/bin/bash

threadNums=(1 2 4 8 14 16 28 56)

executable=$1
inputXML=$2

outputFile=$3

echo "Running $executable with $inputXML:">>"$outputFile"

for threadCount in "${threadNums[@]}";do
  printf ".................\nTHREAD: %s\n" "$threadCount">> "$outputFile"
  OMP_NUM_THREADS="$threadCount" "$executable" -i "$inputXML" -pq >> "$outputFile"
done

printf ".................\n\n##########################################################\n\n" >>"$outputFile"