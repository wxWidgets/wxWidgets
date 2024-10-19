# Retrieve locale data from Windows

echo "=== Retrieve known locales from Windows..."

# Create backup copies
Copy-Item -Path "windows\win-locale-table-win.txt" -Destination "windows\win-locale-table-win.txt.bak"

# Regenerate Windows locale list
..\util\showlocales >windows\win-locale-table-win.txt

echo "-> File 'win-locale-table-win.txt' generated."
