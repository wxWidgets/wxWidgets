import sys, os, string, glob
import re
from docparser.wxclasses import *
from docparser.wxhtmlparse import *
import wx
    
# HTML macros
html_heading = "<H3><font color=\"red\">%s</font></H3>"

def classToHTML(name, thisclass):
    global outdir, classes
    page = open(os.path.join(outdir, "wx_" + name.lower() + ".html"), "w")
    classname = namespacify_wxClasses(name)
    page.write(thisclass.asHtml())
    page.write("<HR>" + html_heading % "Methods")
    
    methods = thisclass.methods
    if len(thisclass.derivedFrom) > 0:
        for parentclass in thisclass.derivedFrom:
            classname = parentclass.replace("wx.", "wx")
            if classname in classes.keys():
                derivedmethods = classes[classname].methods
                if parentclass in derivedmethods:
                    derivedmethods.pop(parentclass)
                methods.update(derivedmethods)
    
    methodnames = sortMethods(classname, methods.keys())
        
    for method in methodnames:
        page.write("<A href=\"#%s\">%s</A></BR>" % (methods[method].getAnchorName(), method))
        
    page.write("<HR>")
    
    for method in methodnames:
        page.write(methods[method].asHtml())
        page.write("<HR>")
    page.close()
        
def sortMethods(classname, methodnames):
    names = methodnames
    names.sort()
    # bump the constructor to the top of the list.
    if classname in names:
        names.remove(classname)
        names.insert(0, classname)
        
    return names
        
def makeDocString(name, docstring, longdocs=""):
    myname = name.replace("wx.", "wx")
    return "DocStr(%s, \"%s\", \"%s\");\n\n" % (myname, docstring, longdocs)

def classToReST(name, thisclass):
    global restdir
    page = open(os.path.join(restdir, "_" + name + "_docstrings.i"), "w")
    page.write(makeDocString(thisclass.name, thisclass.description))
    
    classname = namespacify_wxClasses(name)
    methodnames = sortMethods(classname, thisclass.methods.keys())
        
    for method in methodnames:
        docstr = makeDocString(name + "::" + method.replace("wx.", "wx"), thisclass.methods[method].asReST())
        page.write(docstr)
        
    page.close()
        

docspath = sys.argv[1]
if not os.path.isdir(docspath):
    # get default directory
    print "Please specify the directory where docs are located."
    
outdir = os.path.join(docspath, outputdir)
if not os.path.exists(outdir):
    os.makedirs(outdir)

restdir = os.path.join(docspath, "docstrings")
if not os.path.exists(restdir):
    os.makedirs(restdir)

classes_page = os.path.join(docspath, "wx_classref.html")
print "docspath: %s" % (classes_page)
if os.path.exists(classes_page):
    
    # first, add namespace conventions to classes page.
    output = open(os.path.join(outdir, os.path.basename(classes_page)), "w")
    output.write("<HTML><HEAD></HEAD><BODY>")
    
    propsfile = open(os.path.join(outdir, "props.py"), "w")
    propsfile.write("import wx\n\n")
    
    # now, change the classes.
    print "parsing wx HTML docs..."
    classes = getClasses(classes_page)
    names = classes.keys()
    names.sort()
    propConflicts = []
    for name in names:
        basename = name.replace("wx", "")
        urlname = "wx_%s.html" % name.lower() 
        output.write("<b><a href=\"%s\">%s</a></b><br>" % (urlname, basename))
        print "creating HTML docs for " + name
        classToHTML(name, classes[name])
        print "creating rest docs for " + name
        classToReST(name, classes[name])
        propsfile.write(classes[name].createProps())
        
    propsfile.close()
    output.write("</BODY></HTML>")
    output.close()

    print "prop conflicts: " + `propConflicts`
        
#for doc in glob.glob(os.path.join(docspath, "wx_*.html")):
#    print "doc is: %s" % (doc)
#    pythonize_doc(doc)
        
        
 