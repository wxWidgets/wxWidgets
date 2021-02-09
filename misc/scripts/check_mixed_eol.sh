#!/bin/bash

cd `dirname "$0"`/../..

find . \(                         \
    -path './.git'      -prune -o \
    -path './3rdparty'  -prune -o \
    -path './src/expat' -prune -o \
    -path './src/jpeg'  -prune -o \
    -path './src/png'   -prune -o \
    -path './src/tiff'  -prune -o \
    -path './src/zlib'  -prune \) \
    -o -type f -print0 |
(
while IFS= read -r -d '' file; do
    case "$file" in
    *.ani | *.bmp | *.chm | *.dia | *.gif | *.gz | *.hlp | *.icns | *.ico | *.jpg | *.mo | *.mpg | *.pcx | *.pdf | *.png | *.pnm | *.tga | *.tif | *.ttf | *.wav | *.zip )
        continue
        ;;
    esac

    read dos unix mac <<< $(dos2unix --info=dum "$file" | awk '{print $1, $2, $3}')
    if [[ "$dos" -eq "0" ]] && [[ "$unix" -eq "0" ]]; then
        :
    elif [[ "$dos" -eq "0" ]] && [[ "$mac" -eq "0" ]]; then
        :
    elif [[ "$unix" -eq "0" ]] && [[ "$mac" -eq "0" ]]; then
        :
    else
        echo "$file has mixed EOL"
        rc=$((rc+1))
    fi
done
exit $rc
)
