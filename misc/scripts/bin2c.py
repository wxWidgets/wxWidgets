#!/usr/bin/env python
#
# This is a slightly generalized version of png2c.py and is based on it.

import sys
import os
import os.path
import re
import array
import zlib

args = sys.argv[1:]
compress = len(args) > 0 and args[0] == "--zlib"
if compress:
        args = args[1:]

if len(args) < 1:
        print("""Usage: bin2c [--zlib] binary_file...

Output input files data as C arrays to standard output.

With --zlib, the data is compressed using zlib format first.""")
        sys.exit(1)

r = re.compile("^([a-zA-Z._][a-zA-Z._0-9]*)[.]([a-zA-Z_0-9]+)$")
for path in args:
        filename = os.path.basename(path).replace('-','_')
        m = r.match(filename)

        # Allow only filenames that make sense as C variable names
        if not(m):
                print("Skipped file (unsuitable filename): " + filename)
                continue

        # Read file as character array
        data = open(path, "rb").read()
        if compress:
                comment = "%s - %d bytes, zlib-compressed" % (filename, len(data))
                data = zlib.compress(data, 9)
        else:
                comment = "%s - %d bytes" % (filename, len(data))

        bytes = array.array('B', data)
        count = len(bytes)

        # Create the C header
        text = "/* %s */\n" \
               "static const unsigned char %s_%s_data[] = {\n" % (
                    comment, m.group(1), m.group(2))

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
