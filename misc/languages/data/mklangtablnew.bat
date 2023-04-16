rem Batch script to regenerate the list of known locales

rem Create a copy of the current lists
copy ..\langtabl.txt temp\langtabl_current.txt
copy ..\scripttabl.txt temp\scripttabl_current.txt
copy ..\synonymtabl.txt temp\synonymtabl_current.txt

rem Generate lists based on Unicode data
luashell unicode\uni_territorycodes.lua
luashell unicode\uni_genscriptmap.lua

rem Generate list of locales based on known Windows locales
luashell windows\win_genlocaletable.lua

rem Generate lists based on current wxWidgets data
rem   Extract current wxWidgets version
luashell wx\wx_readversion.lua
rem   Load current version of langtabl.txt
luashell wx\wx_loadlangtabl.lua
rem   Load list of current synonyms
luashell wx\wx_loadsynonymtabl.lua

rem Generate locale data database
del localedata.db3
sqlite3 localedata.db3 ".read mkdatabase.sql"
