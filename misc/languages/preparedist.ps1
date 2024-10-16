# Prepare language data files update artifact

# Generated source files
New-Item -Path "distlang\include\wx\private" -ItemType Directory
New-Item -Path "distlang\interface\wx" -ItemType Directory
New-Item -Path "distlang\src\common" -ItemType Directory
Copy-Item -Path "include\wx\language.h" -Destination "distlang\include\wx\language.h" -Force
Copy-Item -Path "include\wx\private\lang_*.h" -Destination "distlang\include\wx\private" -Force
Copy-Item -Path "interface\wx\language.h" -Destination "distlang\interface\wx\language.h" -Force
Copy-Item -Path "src\common\languageinfo.cpp" -Destination "distlang\src\common\languageinfo.cpp" -Force

# Raw data tables
New-Item -Path "distlang\misc\languages" -ItemType Directory
Copy-Item -Path "misc\languages\*.txt" -Destination "distlang\misc\languages" -recurse -Force
