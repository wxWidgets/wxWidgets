#!/usr/bin/env python

# This script is a slightly modified version of the original found at
#
#       https://wiki.wxwidgets.org/Embedding_PNG_Images-Bin2c_In_Python
#
# without any copyright attribution so it is assumed it can be used under
# wxWindows licence as the rest of the wiki material.

import sys
import os
import os.path
import re
import array

USAGE = """Usage: png2c [-s] [file...]
Output input PNG files as C arrays to standard output. Used to embed PNG images
in C code (like XPM but with full alpha channel support).

  -s    embed the image size in the image names in generated code."""

if len(sys.argv) < 2:
        print(USAGE)
        sys.exit(1)

r = re.compile("^([a-zA-Z._][a-zA-Z._0-9]*)[.][pP][nN][gG]$")

# Automatically replace some symbols in the filenames.
sanitize = str.maketrans('-@', '__')

with_size = 0
size_suffix = ''
for path in sys.argv[1:]:
        if path == '-s':
            with_size = 1
            continue

        filename = os.path.basename(path).translate(sanitize)
        m = r.match(filename)

        # Allow only filenames that make sense as C variable names
        if not(m):
                print("Skipped file (unsuitable filename): " + filename, file=sys.stderr)
                continue

        # Read PNG file as character array
        bytes = array.array('B', open(path, "rb").read())
        count = len(bytes)

        # Check that it's actually a PNG to avoid problems when loading it
        # later.
        #
        # Each PNG file starts with a 8 byte signature that should be followed
        # by IHDR chunk which is always 13 bytes in length so the first 16
        # bytes are fixed (or at least we expect them to be).
        if bytes[0:16].tobytes() != b'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR':
                print('"%s" doesn\'t seem to be a valid PNG file.' % filename)
                continue

        # Try to naively get its size if necessary
        if with_size:
                def getInt(start):
                        """ Convert 4 bytes in network byte order to an integer. """
                        return 16777216*bytes[start]   + \
                                  65536*bytes[start+1] + \
                                    256*bytes[start+2] + \
                                        bytes[start+3];

                size_suffix = "_%dx%d" % (getInt(16), getInt(20))

        # Create the C header
        text = "/* %s - %d bytes */\n" \
               "static const unsigned char %s%s_png[] = {\n" % (
                    filename, count, m.group(1), size_suffix)

        # Iterate the characters, we want
        # lines like:
        #   0x01, 0x02, .... (8 values per line maximum)
        i = 0
        count = len(bytes)
        for byte in bytes:
                # Every new line starts with two whitespaces
                if (i % 8) == 0:
                        text += "  "
                # Then the hex data (up to 8 values per line)
                text += "0x%02x" % (byte)
                # Separate all but the last values
                if (i % 8) == 7:
                        text += ',\n'
                elif (i + 1) < count:
                        text += ", "
                i += 1

        # Now conclude the C source
        text += "};"

        print(text)
