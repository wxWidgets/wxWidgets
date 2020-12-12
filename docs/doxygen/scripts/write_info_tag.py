#
# write_info_tag.py
# Write a key/value pair to an Info.plist file created by Doxygen
# while generating docsets.
#
# Author: Ian McInerney (https://github.com/imciner2)

import sys
import plistlib

# The first argument is the plist filename without the extension
fname = sys.argv[1] + ".plist"

# The second argument is the key to replace
key = sys.argv[2]

# The third argument is the value of the key
val = sys.argv[3]

# Handle boolean values
if val.lower() == "true":
  val = True
elif val.lower() == "false":
  val = False


if sys.version_info >= (3, 4, 0):
  # Use the new API if python 3.4 is used
  with open( fname, 'rb' ) as plist_file:
    pl = plistlib.load( plist_file )

  pl[key] = val

  with open( fname, 'wb' ) as plist_file:
    pl = plistlib.dump( pl, plist_file )

else:
  # Use the old API otherwise (supports python 2.7 as well)
  pl = plistlib.readPlist( fname )
  pl[key] = val
  plistlib.writePlist( pl, fname )
