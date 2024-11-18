# Generate SQL files

echo "=== Generate SQL files..."

# Create a copy of the current wxWidgets tables
echo "-> Create backup copies"
Copy-Item -Path "..\langtabl.txt" -Destination "temp\langtabl_current.txt"
Copy-Item -Path "..\scripttabl.txt" -Destination "temp\scripttabl_current.txt"
Copy-Item -Path "..\synonymtabl.txt" -Destination "temp\synonymtabl_current.txt"
Copy-Item -Path "..\likelytabl.txt" -Destination "temp\likelytabl_current.txt"
Copy-Item -Path "..\matchingtabl.txt" -Destination "temp\matchingtabl_current.txt"
Copy-Item -Path "..\regiongrouptabl.txt" -Destination "temp\regiongrouptabl_current.txt"

# Generate lists based on Unicode data

echo "-> Generate SQL for territory codes"
Invoke-Expression "$env:luashell unicode\uni_territorycodes.lua"

echo "-> Generate SQL for script codes"
Invoke-Expression "$env:luashell unicode\uni_genscriptmap.lua"

echo "-> Generate SQL for likely subtags"
Invoke-Expression "$env:luashell unicode\uni_getlikely.lua"

echo "-> Generate SQL for matching language tags"
Invoke-Expression "$env:luashell unicode\uni_getmatch.lua"

echo "-> Generate SQL for region groups"
Invoke-Expression "$env:luashell unicode\uni_getregions.lua"

# Generate list of locales based on known Windows locales
echo "-> Generate SQL for known Windows locales"
Invoke-Expression "$env:luashell windows\win_genlocaletable.lua"

# Generate lists based on current wxWidgets data

#   Extract current wxWidgets version
echo "-> Generate SQL for wxWidgets version"
Invoke-Expression "$env:luashell wx\wx_readversion.lua"

#   Load current version of langtabl.txt
echo "-> Generate SQL for current wxWidgets language database"
Invoke-Expression "$env:luashell wx\wx_loadlangtabl.lua"

#   Load list of current synonyms
echo "-> Generate SQL for current wxWidgets language synonym table"
Invoke-Expression "$env:luashell wx\wx_loadsynonymtabl.lua"

echo "=== ...SQL files generated."
