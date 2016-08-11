#!/bin/bash

# Author: wang qixiang (wangqx@mpreader.com)

INPUT_DIR=$1
OUTPUT_MF=$2

if [ "x$INPUT_DIR" = "x" ]; then
    INPUT_MF=.
fi

if [ "x$OUTPUT_DIR" = "x" ]; then
    OUTPUT_MF="Makefile"
fi

echo "$INPUT_MF : $OUTPUT_MF"

