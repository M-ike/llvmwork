#!/bin/sh

BIN=../../driver
SRC=.

file_list=""

for file in `find $SRC -name "*.c"`
do
    file_list=$file_list$file" "
done

echo $file_list
$BIN -I. $file_list
