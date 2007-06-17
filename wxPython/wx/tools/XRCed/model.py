# Name:         model.py
# Purpose:      Model class and related
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      07.06.2007
# RCS-ID:       $Id$

import os,sys
from xml.dom import minidom
from globals import *

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

# Model object is used for data manipulation
class _Model:
    def __init__(self):
        self.dom = None

    def init(self, dom=None):
        self.path = ''
        if self.dom: self.dom.unlink()
        if not dom:
            self.dom = MyDocument()
            self.mainNode = self.dom.createElement('resource')
            self.dom.appendChild(self.mainNode)
        else:
            self.dom = dom
            self.mainNode = dom.documentElement
        # Dummy element used for temporarily renaming noname nodes
        self.testElem = self.dom.createElement('dummy')
        self.mainNode.appendChild(self.testElem)

    def loadXML(self, path):
        f = open(path)
        dom = minidom.parse(f)
        f.close()
        self.init(dom)

        # Set encoding global variable and default encoding
        if dom.encoding:
            g.currentEncoding = dom.encoding
            wx.SetDefaultPyEncoding(g.currentEncoding.encode())
        else:
            g.currentEncoding = ''
        
        # Change dir
        self.path = path = os.path.abspath(path)
        dir = os.path.dirname(path)
        if dir: os.chdir(dir)

    def saveXML(self, path):
        if g.currentEncoding:
            import codecs
            f = codecs.open(path, 'wt', g.currentEncoding)
        else:
            f = open(path, 'wt')
        # Make temporary copy for formatting it
        domCopy = MyDocument()
        mainNode = domCopy.appendChild(self.mainNode.cloneNode(True))
        # Remove first child (testElem)
        testElem = mainNode.firstChild
        mainNode.removeChild(testElem)
        testElem.unlink()
        self.indent(mainNode)
        domCopy.writexml(f, encoding = g.currentEncoding)
        f.close()
        domCopy.unlink()

    def indent(self, dom):
        print 'NYI'

    def createObjectNode(self, className):
        node = self.dom.createElement('object')
        node.setAttribute('class', className)
        return node

Model = _Model()
