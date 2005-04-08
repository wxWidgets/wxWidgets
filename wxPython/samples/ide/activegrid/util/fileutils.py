#----------------------------------------------------------------------------
# Name:         fileutils.py
# Purpose:      Active grid miscellaneous utilities
#
# Author:       Jeff Norton
#
# Created:      12/10/04
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import os

def createFile(filename, mode='w'):
    f = None
    try:
        f = file(filename, mode)
    except:
        os.makedirs(filename[:filename.rindex(os.sep)])
        f = file(filename, mode)
    return f

def compareFiles(file1, file2):
    file1.seek(0)
    file2.seek(0)
    while True:
        line1 = file1.readline()
        line2 = file2.readline()
        if (len(line1) == 0):
            if (len(line2) == 0):
                return 0
            else:
                return -1
        elif (len(line2) == 0):
            return -1
        elif (line1 != line2):
            return -1

