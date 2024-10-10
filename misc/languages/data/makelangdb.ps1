# Create SQLite language database and generate language database files

echo "=== Create SQLite language database and generate language database files..."

# Delete old database, if it exists
Remove-Item -Path "localedata.db3" -ErrorAction Ignore

# Generate new database and new language database files
Invoke-Expression "sqlite3 localedata.db3 `".read mkdatabase.sql`""

echo "-> SQLite database and language database files generated."
