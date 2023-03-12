#!bash

# fileslist=$(find mcr src -name '*.h' -or -name '*.c' -or -name '*.cpp')
fileslist=$(find mcr src -type f)
echo "
files list =
$fileslist"

echo "
number of files="
echo "$fileslist" | wc -l

wordcount=$(echo "$fileslist" | xargs wc -w)
echo "
word count =
$wordcount"

linecount=$(echo "$fileslist" | xargs wc -l)
echo "
line count =
$linecount"

