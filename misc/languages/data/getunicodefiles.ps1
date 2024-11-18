# Download files from Unicode organization
#
# This script has 2 parameters:
# 1. $cldrRelease [mandatory] - The CLDR release number (i.e. "44-1", "45", "46-beta2" ...)
# 2. -useCurrent  [optional]  - Download the latest file versions (git HEAD)
param([Parameter(Mandatory=$true)][string]$cldrRelease,[switch] $useCurrent)

echo "=== Download of Unicode files start..."

# Create backup copies
echo "-> Create backup copies"
Copy-Item -Path "unicode\iso15924.txt" -Destination "unicode\iso15924.txt.bak"
Copy-Item -Path "unicode\territory_codes.txt" -Destination "unicode\territory_codes.txt.bak"
Copy-Item -Path "unicode\likelySubtags.xml" -Destination "unicode\likelySubtags.xml.bak"
Copy-Item -Path "unicode\languageInfo.xml" -Destination "unicode\languageInfo.xml.bak"
Copy-Item -Path "unicode\supplementalData.xml" -Destination "unicode\supplementalData.xml.bak"

# Set base URL path for download
if ($useCurrent)
{
  # Download current data
  $basePath="https://github.com/unicode-org/cldr/raw/main"
}
else
{
  # Download data of a specific release
  $basePath="https://github.com/unicode-org/cldr/raw/refs/tags/release-$cldrRelease"
}
echo "-> URL base path: $basePath"

# Download ISO 15924 (list of scripts)
Invoke-WebRequest -Uri "https://www.unicode.org/iso15924/iso15924.txt" -OutFile "unicode\iso15924.txt"
echo "-> Download of iso15924.txt completed."

# Download ISO 3166 (list of territories)
Invoke-WebRequest -Uri "$basePath/tools/cldr-code/src/main/resources/org/unicode/cldr/util/data/territory_codes.txt" -OutFile "unicode\territory_codes.txt"
echo "-> Download of territory_codes.txt completed."

# Download list of likely subtags
Invoke-WebRequest -Uri "$basePath/common/supplemental/likelySubtags.xml" -OutFile "unicode\likelySubtags.xml"
echo "-> Download of likelySubtags.xml completed."

# Download list of language infos
Invoke-WebRequest -Uri "$basePath/common/supplemental/languageInfo.xml" -OutFile "unicode\languageInfo.xml"
echo "-> Download of languageInfo.xml completed."

# Download list of language infos
Invoke-WebRequest -Uri "$basePath/common/supplemental/supplementalData.xml" -OutFile "unicode\supplementalData.xml"
echo "-> Download of supplementalData.xml completed."

# Remove whitespace from data files
echo "-> Remove whitespace from text files"
Invoke-Expression "$env:luashell removewhitespace.lua unicode/iso15924.txt"
Invoke-Expression "$env:luashell removewhitespace.lua unicode/territory_codes.txt"

echo "... Download completed."
