#----------------------------------------------------------------------------
# Name:         strutils.py
# Purpose:      String Utilities
#
# Author:       Morgan Hua
#
# Created:      11/3/05
# CVS-ID:       $Id$
# Copyright:    (c) 2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------


def caseInsensitiveCompare(s1, s2):
    """ Method used by sort() to sort values in case insensitive order """
    s1L = s1.lower()
    s2L = s2.lower()
    if s1L == s2L:
        return 0
    elif s1L < s2L:
        return -1
    else:
        return 1
