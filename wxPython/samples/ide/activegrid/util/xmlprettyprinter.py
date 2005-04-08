#----------------------------------------------------------------------------
# Name:         xmlprettyprinter.py
# Purpose:
#
# Author:       John Spurling
#
# Created:      9/21/04
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------
import xml.sax
import xml.sax.handler


class XMLPrettyPrinter(xml.sax.ContentHandler):
    def __init__(self, indentationChar='    ', newlineChar='\n'):
        self.xmlOutput = ''
        self.indentationLevel = 0
        self.indentationChar = indentationChar
        self.elementStack = []
        self.newlineChar = newlineChar
        self.hitCharData = False

    ## ContentHandler methods
    def startElement(self, name, attrs):
        indentation = self.newlineChar + (self.indentationLevel * self.indentationChar)
        # build attribute string
        attrstring = ''
        for attr in attrs.getNames():
            value = attrs[attr]
            attrstring += ' %s="%s"' % (attr, value)
        self.xmlOutput += '%s<%s%s>' % (indentation, name, attrstring)
        self.indentationLevel += 1
        self.elementStack.append(name)
        self.hitCharData = False

    def characters(self, content):
        self.xmlOutput += content
        self.hitCharData = True

    def endElement(self, name):
        self.indentationLevel -= 1
        indentation = ''
        if not self.hitCharData:
##            indentation += self.newlineChar + (self.indentationLevel * self.indentationChar)
            indentation += self.indentationLevel * self.indentationChar
        else:
            self.hitCharData = False
        self.xmlOutput += '%s</%s>%s' % (indentation, self.elementStack.pop(), self.newlineChar)

    def getXMLString(self):
        return self.xmlOutput[1:]

def xmlprettyprint(xmlstr, spaces=4):
    xpp = XMLPrettyPrinter(indentationChar=' ' * spaces)
    xml.sax.parseString(xmlstr, xpp)
    return xpp.getXMLString()

if __name__ == '__main__':
    simpleTestString = """<one>some text<two anattr="booga">two's data</two></one>"""
    print prettyprint(simpleTestString)

