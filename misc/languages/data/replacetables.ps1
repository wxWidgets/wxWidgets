# Replace the wxWidgets data tables by new versions

echo "=== Replace previous versions with new versions..."

Copy-Item -Path "langtabl.txt" -Destination "..\langtabl.txt" -Force
Copy-Item -Path "scripttabl.txt" -Destination "..\scripttabl.txt" -Force
Copy-Item -Path "synonymtabl.txt" -Destination "..\synonymtabl.txt" -Force
Copy-Item -Path "likelytabl.txt" -Destination "..\likelytabl.txt" -Force
Copy-Item -Path "matchingtabl.txt" -Destination "..\matchingtabl.txt" -Force
Copy-Item -Path "regiongrouptabl.txt" -Destination "..\regiongrouptabl.txt" -Force

echo "-> wxWidgets language data tables replaced by new version."
