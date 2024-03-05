#!/bin/bash

cd $(dirname "$0")/../..

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

    # skip binary files
    case "$file" in
    *.ani | *.bmp | *.chm | *.cur | *.dia | *.gif | *.gz | *.hlp | *.icns | *.ico | *.jpg | *.mo | *.mpg | *.pcx | *.pdf | *.png | *.pnm | *.pyc | *.tga | *.tif | *.ttf | *.wav | *.zip )
        continue
        ;;
    esac

    # get used EOL
    read dos unix mac <<< $(dos2unix --info=dum "$file" | awk '{print $1, $2, $3}')
    if [[ "$dos" -eq 0 ]] && [[ "$unix" -eq 0 ]]; then
        :
    elif [[ "$dos" -eq 0 ]] && [[ "$mac" -eq 0 ]]; then
        :
    elif [[ "$unix" -eq 0 ]] && [[ "$mac" -eq 0 ]]; then
        :
    else
        echo "ERROR - mixed EOL: $file"
        rc=$((rc+1))
        continue
    fi

    # empty file
    if [[ "$dos" -eq 0 ]] && [[ "$unix" -eq 0 ]] && [[ "$mac" -eq 0 ]]; then
        continue;
    fi

    # determine expected EOL
    warn_expected_eol_unknown=0
    case "$file" in
    *.applescript | *.apspec | *.bkgen | *.bkl | *.c | *.C | *.cmake | *.cpp | *.css | *.cxx | *.guess | *.h | *.htm | *.html | *.iface | *.in | *.js | *.json | *.log | *.lua | *.m4 | *.mm | *.manifest | *.md | *.mk | *.mms | *.opt | *.patch | *.pbxproj | *.pl | *.plist | *.po | *.py | *.sh | *.sub | *.svg | *.tex | *.txt | *.TXT | *.unx | *.vms | *.xcconfig | *.xcscheme | *.xcworkspacedata | *.xbm | *.xml | *.xpm | *.xrc | *.yml )
        expected_eoltype=unix
        ;;
    *.bat | *.bcc | *.gcc | *.iss | *.props | *.rc | *.sln | *.vc | *.vcproj | *.vcxproj | *.vcxproj.filters )
        expected_eoltype=dos
        ;;
    * )
        # warn_expected_eol_unknown=1
        expected_eoltype=
        ;;
    esac

    # check for filename without extension
    if [[ "$expected_eoltype" = "" ]]; then
        fullfile=$(basename -- "$file")
        if [[ "${fullfile:1}" != *"."* ]]; then
            warn_expected_eol_unknown=0
            expected_eoltype=unix
        fi
    fi

    if [[ "$warn_expected_eol_unknown" -eq 1 ]]; then
        echo "WARNING - no expected EOL specified: $file"
    fi

    # check if expected EOL matches used EOL
    if [[ "$expected_eoltype" = "unix" ]] && [[ "$unix" -eq 0 ]]; then
        echo "ERROR - wrong EOL, expected unix: $file"
        rc=$((rc+1))
    elif [[ "$expected_eoltype" = "dos" ]] && [[ "$dos" -eq 0 ]]; then
        echo "ERROR - wrong EOL, expected dos: $file"
        rc=$((rc+1))
    fi

done
exit $rc
)
