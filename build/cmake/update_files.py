#!/usr/bin/env python

#############################################################################
# Name:        build/cmake/update_files.py
# Purpose:     Convert build/files to files.cmake
# Author:      Tobias Taschner
# Created:     2016-09-20
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

import os
import re

outpath = os.path.dirname(os.path.abspath(__file__))

infile = open(outpath + "/../files", "r")
outfile = open(outpath + "/files.cmake", "w")
outfile.write("# Automatically generated from build/files by " + os.path.basename(__file__) + "\n")
outfile.write("# DO NOT MODIFY MANUALLY !\n\n")

# Compile regular expressions
var_ex = re.compile('([\w]+)[\s]*=')
comment_ex = re.compile('^[#]+')
evar_ex = re.compile('\$\(([\w]+)\)')
cmd_ex = re.compile('^<')

files = None
var_name = None

def write_file_list():
    # Write current list of files to output file
    if not var_name:
        return

    outfile.write('set(' + var_name + '\n')
    for file in files:
        outfile.write('    ')
        vm = evar_ex.match(file)
        if vm:
            # Convert variable reference to cmake variable reference
            outfile.write('${'+vm.group(1)+'}')
        else:
            outfile.write(file)
        outfile.write('\n')

    outfile.write(')\n\n')

for line in infile.readlines():
    # Ignore comment lines
    m = comment_ex.match(line)
    if m:
        continue
    m = cmd_ex.match(line.strip())
    if m:
        # Ignore bake file commands but note them in the target file in
        # case we might need them
        line = '#TODO: ' + line

    # Check for variable declaration
    m = var_ex.match(line)
    if m:
        write_file_list()

        var_name = m.group(1)
        files = []
    else:
        # Collect every file entry
        file = line.strip()
        if file and var_name:
            files.append(file)

# Write last variable
write_file_list()

infile.close()
outfile.close()
