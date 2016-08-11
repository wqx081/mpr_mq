#!/bin/bash

# Author: wang qixiang (wangqx@mpreader.com)

if [ -z "$1" ]; then
    INPUT_DIR="."
else
    INPUT_DIR=$1
fi


if [ -z "$2" ]; then
    OUTPUT_MF="Makefile"
else
    OUTPUT_MF=$2
fi

#echo "$INPUT_DIR : $OUTPUT_MF"

echo "Starting."

CPP_SOURCES=`find $INPUT_DIR -regex '.*\.\(cpp\|cc\)'`
C_SOURCES=`find $INPUT_DIR -regex '.*\.\(c\)'`
HEADERS=`find $INPUT_DIR -regex '.*\.\(h\|hpp\)'`


echo "$CPP_SOURCES"
echo "================="
echo "$C_SOURCES"
echo "================="
echo "$HEADERS"


echo ""
echo "Done."
