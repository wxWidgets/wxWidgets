# Suppresses some of the more common warnings

lacheck $1 | sed -e "/possible unwanted/d;/you may need/d;/TAB characyer/d;/Use \`/d;/missing \`\\\/d;/Dots/d;/Whitespace/d"
