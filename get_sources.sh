#!/bin/bash

DIR="."

if [[ -e "src" ]]
then
	DIR="src"
fi


echo "SOURCES += \\"
find $DIR -follow -name '*.cpp' -o -name '*.c' | sed 's/$/ \\/' | sed 's/^/        /'  | sort -u
echo " "

echo "HEADERS += \\"
find $DIR -follow -name '*.h' | sed 's/$/ \\/' | sed 's/^/        /' | sort -u
echo " "
