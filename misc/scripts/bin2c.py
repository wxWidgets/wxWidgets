#!/usr/bin/env python
#
# This is a slightly generalized version of png2c.py and is based on it.

import sys
import os
import os.path
import re
import array

if len(sys.argv) < 2:
        print("""Usage: bin2c binary_file...

Output input files data as C arrays to standard output.""")
        sys.exit(1)

r = re.compile("^([a-zA-Z._][a-zA-Z._0-9]*)[.]([a-zA-Z_0-9]+)$")
for path in sys.argv[1:]:
        filename = os.path.basename(path).replace('-','_')
        m = r.match(filename)

        # Allow only filenames that make sense as C variable names
        if not(m):
                print("Skipped file (unsuitable filename): " + filename)
                continue

        # Read file as character array
        bytes = array.array('B', open(path, "rb").read())
        count = len(bytes)

        # Create the C header
        text = "/* %s - %d bytes */\n" \
               "static const unsigned char %s_%s_data[] = {\n" % (
                    filename, count, m.group(1), m.group(2))

        i = 0
        for byte in bytes:
                if i % 16 == 0:
                        text += "\n   "
                i += 1
                text += " 0x%02x" % byte
                if i != count:
                        text += ","

        text += "\n};"

        print(text)
