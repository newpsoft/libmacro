#!/bin/bash

# fileslist=$(find mcr src -name '*.h' -or -name '*.c' -or -name '*.cpp')
fcmd=(find mcr src -type f)
fileslist=$(${fcmd[@]})
echo "
files list =
$fileslist"

echo "
number of files="
wc -l <<< ${fileslist}

wordcount=$(${fcmd[@]} -print0 | xargs -0 wc -w)
echo "
word count =
$wordcount"

linecount=$(${fcmd[@]} -print0 | xargs -0 wc -l)
echo "
line count =
$linecount"

