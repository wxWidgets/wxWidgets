##############################################################################
# Name:       misc/scripts/update_doc_libs.py
# Purpose:    Automatically insert \Library{} headers in the doc files
# Created:    2007-07-28
# RCS-ID:     $Id: makeunixtags.sh 46320 2007-06-04 11:02:29Z VZ $
# Copyright:  (c) 2007 Francesco Montorsi
# Licence:    wxWindows licence
##############################################################################

import sys, os, glob, distutils.file_util

tofix = set()
count = 0
for f in glob.glob('*.tex'):
    file = open(f, "r")
    if not file:
        print "could not open %s" % f
        continue
    print "opened file %s" % f
    count = count + 1
    
    # search \class
    content = file.readlines()
    classdecl = 0
    for i in range(len(content)):
        line = content[i]
        if "\class{" in line:
            classdecl = classdecl + 1
            
            classname = line
            classname = classname[classname.find("\class{"):]
            classname = classname[classname.find("{")+1:classname.find("}")]
            print " the class declared is named '%s'" % classname
            
            tofix.add(f)     # consider this .tex broken
            
            # now search the include file for this class
            include = ""
            for j in range(i,len(content)):
                line = content[j]
                if "wx/" in line and ".h" in line:
                    include = line[line.find("wx/"):line.find(".h")+2]
                    break
            if include == "":
                print " no include file declared for class %s" % classname
                continue
            
            include = include.replace("\\_", "_")
            print " the include file for %s is %s" % (classname, include)
            
            # now try to understand which libs contains this class
            include = "../../../include/" + include
            header = open(include, "r")
            if not file:
                print " could not open %s" % include
                continue

            decl = ""
            content2 = header.readlines()
            
            # if they exist append port-specific headers contents
            for c in ["wx/gtk/", "wx/msw/", "wx/mac/", "wx/generic/"]:
                try:
                    temp = include.replace("wx/", c)
                    print " trying to open %s..." % temp
                    header = open(temp, "r")
                    headercontents = header.readlines()
                    content2 = content2 + headercontents
                    print " added %d lines from %s" % (len(headercontents), temp)
                except:
                    pass
            
            for line in content2:
                if "class " in line and classname in line:
                    if line.find("class") < line.find(classname):  # could be a comment
                        if "_" in line:
                            decl = line[line.find("_")+1:]
                            decl = decl[:decl.find(" ")]
                            decl = decl.replace("FWD_", "")
                            decl = decl[0:1].upper() + decl[1:].lower()
                            break
                        elif " WXDLLEXPORT " in line:
                            decl = "Core"
                            break
                        
            if decl == "":
                print " no declaration associated with %s" % classname
                continue
            
            print " the declaration associated with %s is %s" % (classname, decl)
            tofix.remove(f) # was a valid .tex (at least for current class)
            
            # now modify the .tex file
            content.insert(j+2, "\wxheading{Library}\n\n\helpref{wx%s}{librarieslist}\n\n" % decl)
            
            # write it
            file = open(f, "w")
            file.write(''.join(content))
            file.flush()
            
            file = open(f, "r")

            print " updated %s" % f

            
    print " file %s contains %d class declarations" % (f, classdecl)

print "\nProcessed %d files." % count
print "There are %d files to fix:\n%s" % (len(tofix), '\n'.join(tofix))
