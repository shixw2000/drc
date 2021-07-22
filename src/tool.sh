#!/bin/bash

inFile=$1
outFile=$2

if [ -z $1 ]
then
    echo "enter a in file"
    exit -1
fi

if [ -z $2 ]
then
    echo "enter a out file"
    exit -2
fi

awk '/sq_distance/{$1="";print $0}' $inFile|sort -t = -k 6n -o $outFile

