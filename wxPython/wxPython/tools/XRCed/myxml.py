# Name:         myxml.py
# Purpose:      XRC editor, XML stuff
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      15.10.2002
# RCS-ID:       $Id$

from xml.dom import minidom

# Redefine writing to include encoding
class MyDocument(minidom.Document):
    def __init__(self):
        minidom.Document.__init__(self)
        self.encoding = ''
    def writexml(self, writer, indent="", addindent="", newl="", encoding=""):
        if encoding: encdstr = 'encoding="%s"' % encoding
        else: encdstr = ''
        writer.write('<?xml version="1.0" %s?>\n' % encdstr)
        for node in self.childNodes:
            node.writexml(writer, indent, addindent, newl)
