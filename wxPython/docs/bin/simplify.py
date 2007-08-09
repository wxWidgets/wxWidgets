#!/usr/bin/python
#---------------------------------------------------------------------------
#
# Like simplify.xsl but using Python so a few non-standard conversions can
# also be done.  (Currently it is still about the same as simplify.xsl...)
#
#---------------------------------------------------------------------------

import sys
import os
import libxml2


DEST="docs/xml/wxPython-metadata.xml"
SRC="docs/xml-raw"


classMap = {
    'wxString'   : 'String',
    'void'       : '',
    }


def getModuleNames():
    """
    Get the list of extension modules from setup.py
    """
    import setup
    names = [e.name[1:] for e in setup.wxpExtensions]
    return names



def getAttr(node, name):
    """
    Get a value by name from the <attribute> elements in the SWIG XML output
    """
    path = "./attributelist/attribute[@name='%s']/@value" % name
    n = node.xpathEval2(path)
    if len(n):
        return n[0].content
    else:
        return None


def fixType(typeStr):
    """
    Fixup type string, dropping the swig pointer and other flags
    """
    pos = typeStr.rfind('.')
    if pos != -1:
        typeStr = typeStr[pos+1:]
    return classMap.get(typeStr, typeStr)


def processModule(newDocNode, modulename):
    """
    Start processing a new XML file, create a module element and then
    find the include elements
    """
    filename = os.path.join(SRC, "%s_swig.xml" % modulename)
    print filename

    doc = libxml2.parseFile(filename)
    topNode = doc.getRootElement()

    # make a module element
    name = getAttr(topNode, "module")
    ##assert name == modulename # sanity check    

    moduleNode = libxml2.newNode("module")
    moduleNode.setProp("name", name)    
    newDocNode.addChild(moduleNode)

    node = topNode.children
    while node is not None:
        if node.name == "include":
            processInclude(moduleNode, node)
        node = node.next

    doc.freeDoc()



def processInclude(moduleNode, includeNode):
    """
    Almost everything we are interested in is inside an <include>,
    which may also be nested.
    """

    # check first for imports
    for node in includeNode.xpathEval2("import"):
        try:
            modNode = node.xpathEval2("module")[0]
            name = getAttr(modNode, "name")
            impNode = libxml2.newNode("import")
            impNode.setProp("name", name)
            moduleNode.addChild(impNode)
        except IndexError:
            pass
        
    # then look through the child nodes for other things we need
    node = includeNode.children
    while node is not None:
        if node.name == "insert":
            processInsert(moduleNode, node)

        elif node.name == "class":
            processClass(moduleNode, node)

        elif node.name == "cdecl" and getAttr(node, "view") == "globalfunctionHandler":
            func = libxml2.newNode("method")
            func.setProp("name",    getAttr(node, "sym_name"))
            func.setProp("oldname", getAttr(node, "name"))
            func.setProp("type",    fixType(getAttr(node, "type")))
            doCheckOverloaded(func, node)
            doDocStrings(func, node)
            doParamList(func, node)
            moduleNode.addChild(func)
            
                          
        elif node.name == "include":
            processInclude(moduleNode, node)

        node = node.next



def processInsert(parentNode, insertNode):
    """
    Check for pythoncode
    """
    if getAttr(insertNode, "section") == "python":
        code = getAttr(insertNode, "code")
        node = libxml2.newNode("pythoncode")
        node.addChild(libxml2.newText(code))
        parentNode.addChild(node)



def processClass(parentNode, classNode):
    """
    Handle classes, constructors, methods, etc.
    """
    # make class element
    klass = libxml2.newNode("class")
    name = getAttr(classNode, "sym_name")
    oldname = getAttr(classNode, "name")
    classMap[oldname] = name
    klass.setProp("name",    name)
    klass.setProp("oldname", oldname)
    klass.setProp("module",  getAttr(classNode, "module"))
    doDocStrings(klass, classNode)
    parentNode.addChild(klass)

    # check for baseclass(es)
    for node in classNode.xpathEval2("attributelist/baselist/base"):
        baseclass = libxml2.newNode("baseclass")
        basename = node.prop("name")
        baseclass.setProp("name", classMap.get(basename, basename))
        klass.addChild(baseclass)

    # check for constructors/destructors
    for type in ["constructor", "destructor"]:
        for node in classNode.xpathEval2("%s | extend/%s" % (type, type)):
            func = libxml2.newNode(type)
            func.setProp("name", getAttr(node, "sym_name"))
            if parentNode.name != "destructor":
                doCheckOverloaded(func, node)
            doDocStrings(func, node)
            doParamList(func, node)
            klass.addChild(func)

    # check for cdecl's.  In class scope we are interested in methods,
    # static methods, or properties
    for node in classNode.xpathEval2("cdecl | extend/cdecl"):
        view = getAttr(node, "view")
        if view == "memberfunctionHandler":
            func = libxml2.newNode("method")
            func.setProp("name", getAttr(node, "sym_name"))
            func.setProp("type", fixType(getAttr(node, "type")))
            doCheckOverloaded(func, node)
            doDocStrings(func, node)
            doParamList(func, node)
            klass.addChild(func)
            
        elif view == "staticmemberfunctionHandler":
            func = libxml2.newNode("staticmethod")
            func.setProp("name", getAttr(node, "sym_name"))
            func.setProp("type", fixType(getAttr(node, "type")))
            doCheckOverloaded(func, node)
            doDocStrings(func, node)
            doParamList(func, node)
            klass.addChild(func)
        
        elif view == "variableHandler":
            prop = libxml2.newNode("property")
            prop.setProp("name", getAttr(node, "sym_name"))
            prop.setProp("type", fixType(getAttr(node, "type")))
            if getAttr(node, "feature_immutable"):
                prop.setProp("readonly", "yes")
            else:
                prop.setProp("readonly", "no")
            doDocStrings(prop, node)
            klass.addChild(prop)



def doParamList(parentNode, srcNode):
    """
    Convert the parameter list
    """
    params = srcNode.xpathEval2("attributelist/parmlist/parm")
    if params:
        plist = libxml2.newNode("paramlist")
        for p in params:
            pnode = libxml2.newNode("param")
            pnode.setProp("name", getAttr(p, "name"))
            pnode.setProp("type", fixType(getAttr(p, "type")))
            pnode.setProp("default", getAttr(p, "value"))
            plist.addChild(pnode)
        parentNode.addChild(plist)



def doCheckOverloaded(parentNode, srcNode):
    """
    Set an attribute indicating if the srcNode is tagged as being overloaded
    """
    if srcNode.xpathEval2("./attributelist/attribute[@name='sym_overloaded']"):
        parentNode.setProp("overloaded", "yes")
    else:
        parentNode.setProp("overloaded", "no")



def doDocStrings(parentNode, srcNode):
    """
    Check for the various possible docstring attributes, and attach
    coresponding child nodes if found.
    """
    def makeDocElement(name, content):
        node = libxml2.newNode(name)
        node.addChild(libxml2.newText(content))
        return node
    
    autodoc = getAttr(srcNode, "python_autodoc")
    docstr  = getAttr(srcNode, "feature_docstring")
    if autodoc:
        parentNode.addChild(makeDocElement("autodoc", autodoc))
    if docstr:
        parentNode.addChild(makeDocElement("docstring", docstr))
        




def main():
    if not os.path.exists(SRC):
        print "Unable to find %s, please run this script from the root wxPython directory." % SRC
        sys.exit(1)

    newDoc = libxml2.newDoc("1.0")
    newTopNode = libxml2.newNode("wxPython-metadata")
    newDoc.addChild(newTopNode)
    
    for m in getModuleNames():
        processModule(newTopNode, m)

    newDoc.saveFormatFile(DEST, True)
    print "Wrote simplified metadata to", DEST

#---------------------------------------------------------------------------

if __name__ == "__main__":
    main()
