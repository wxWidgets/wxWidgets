"""
Name: doxymlparser.py
Author: Kevin Ollivier
License: wxWidgets License
"""

__description__ = """
Takes the output of Doxygen XML and parses it to retrieve metadata about the classes and methods.

To create the Doxygen XML files, from the wxWidgets/docs/doxygen directory, do:

./regen.sh xml

To see the results from parsing a particular class, do:

python doxymlparser.py --report out/xml/classwx_<whatever>.xml
"""

#!/usr/bin/env python
import optparse
import os
import string

import sys
import types
from xml.dom import minidom

option_dict = { 
            "report"        : (False, "Print out the classes and methods found by this script."),
          }
    
parser = optparse.OptionParser(usage="usage: %prog [options] <doxyml files to parse>\n" + __description__, version="%prog 1.0")

for opt in option_dict:
    default = option_dict[opt][0]
    
    action = "store"
    if type(default) == types.BooleanType:
        action = "store_true"
    parser.add_option("--" + opt, default=default, action=action, dest=opt, help=option_dict[opt][1])

options, arguments = parser.parse_args()

class ClassDefinition:
    def __init__(self):
        self.name = ""
        self.methods = []
        self.brief_description = ""
        self.detailed_description = ""
        self.includes = []
        self.bases = []
        
    def __str__(self):
        str_repr = """
Class: %s
Bases: %s
Includes: %s
Brief Description: 
%s

Detailed Description:
%s
""" % (self.name, string.join(self.bases, ", "), self.includes, self.brief_description, self.detailed_description)
        str_repr += "Methods:\n"
        
        for method in self.methods:
            str_repr += str(method)
        
        return str_repr

class MethodDefinition:
    def __init__(self):
        self.name = ""
        self.return_type = ""
        self.argsstring = ""
        self.definition = ""
        self.params = []
        self.brief_description = ""
        self.detailed_description = ""
        
    def __str__(self):
        str_repr = """
Method: %s
Return Type: %s
Params: %r
Prototype: %s
Brief Description: 
%s

Detailed Description:
%s
""" % (self.name, self.return_type, self.params, self.definition + self.argsstring, self.brief_description, self.detailed_description)
        return str_repr     

def getTextValue(node, recursive=False):
    text = ""
    for child in node.childNodes:
        if child.nodeType == child.ELEMENT_NODE and child.nodeName == "ref":
            text += getTextValue(child)
        if child.nodeType == child.TEXT_NODE:
            # Add a space to ensure we have a space between qualifiers and parameter names
            text += child.nodeValue.strip() + " "
            
    return text.strip()

def doxyMLToText(node):
    return text

class DoxyMLParser:
    def __init__(self):
        self.classes = []

    def parse(self, filename):
        self.xmldoc = minidom.parse(filename).documentElement
        for node in self.xmldoc.getElementsByTagName("compounddef"):
            self.classes.append(self.parse_class(node))
            
    def parse_class(self, class_node):
        new_class = ClassDefinition()
        for node in class_node.childNodes:
            if node.nodeName == "compoundname":
                new_class.name = getTextValue(node)
                print "Parsing class %s" % new_class.name
            elif node.nodeName == "basecompoundref":
                new_class.bases.append(getTextValue(node))
            elif node.nodeName == "briefdescription":
                # let the post-processor determ
                new_class.brief_description = node.toxml()
            elif node.nodeName == "detaileddescription":
                new_class.detailed_description = node.toxml()

        self.parse_methods(new_class, class_node)
        return new_class
        
    def parse_methods(self, new_class, root):
        for method in root.getElementsByTagName("memberdef"):
            new_method = MethodDefinition()
            for node in method.childNodes:
                if node.nodeName == "name":
                    new_method.name = getTextValue(node)
                elif node.nodeName == "type":
                    new_method.return_type = getTextValue(node)
                elif node.nodeName == "definition":
                    new_method.definition = getTextValue(node)
                elif node.nodeName == "argsstring":
                    new_method.argsstring = getTextValue(node)
                elif node.nodeName == "param":
                    param = {}
                    for child in node.childNodes:
                        if child.nodeType == child.ELEMENT_NODE:
                            param[child.nodeName] = getTextValue(child)
                    new_method.params.append(param)
            print "Adding %s" % (new_method.name + new_method.argsstring)
            new_class.methods.append(new_method)

if __name__ == "__main__":
    if len(arguments) < 1:
        parser.print_usage()
        sys.exit(1)
    
    doxyparse = DoxyMLParser()
    for arg in arguments:
        doxyparse.parse(arg)        

    if options.report:
        for aclass in doxyparse.classes:
            print str(aclass)

