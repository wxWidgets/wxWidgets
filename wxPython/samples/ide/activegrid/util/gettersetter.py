#----------------------------------------------------------------------------
# Name:         gettersetter.py
# Purpose:
#
# Author:       Peter Yared
#
# Created:      7/28/04
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------
def gettersetter(list):
    for attr in list:
        lowercase = attr[0].lower() + attr[1:]
        uppercase = attr[0].upper() + attr[1:]
        print "    def get%s(self):" % uppercase
        print "        return self._%s" % lowercase
        print
        print "    def set%s(self, %s):" % (uppercase, lowercase)
        print "        self._%s = %s" % (lowercase, lowercase)
        print

def listgettersetter(list):
    for attr in list:
        lowercase = attr[0].lower() + attr[1:]
        uppercase = attr[0].upper() + attr[1:]
        print "    def get%s(self):" % uppercase
        print "        return self._%s" % lowercase
        print
        print "    def add%s(self, %s):" % (uppercase[:-1], lowercase[:-1])
        print "        self._%s.append(%s)" % (lowercase, lowercase[:-1])
        print
        print "    def remove%s(self, %s):" % (uppercase[:-1], lowercase[:-1])
        print "        self._%s.remove(%s)" % (lowercase, lowercase[:-1])
        print
        
