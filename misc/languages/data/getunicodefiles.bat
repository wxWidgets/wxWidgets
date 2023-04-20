rem Download files from Unicode Org

rem Retrieve working directory
set "workdir=%cd%"

rem Create backup copies
copy unicode\iso15924.txt unicode\iso15924.txt.bak
copy unicode\territory_codes.txt unicode\territory_codes.txt.bak

rem Download ISO 15924 (list of scripts)
bitsadmin /transfer wxDownloadJob /download /priority normal https://www.unicode.org/iso15924/iso15924.txt %workdir%\unicode\iso15924.txt

rem Download ISO 3166 (list of territories)
bitsadmin /transfer wxDownloadJob /download /priority normal https://github.com/unicode-org/cldr/raw/main/tools/cldr-code/src/main/resources/org/unicode/cldr/util/data/territory_codes.txt %workdir%\unicode\territory_codes.txt

luashell removewhitespace.lua unicode/iso15924.txt
luashell removewhitespace.lua unicode/territory_codes.txt
