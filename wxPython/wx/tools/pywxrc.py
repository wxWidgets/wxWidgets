#----------------------------------------------------------------------
# Name:        wx.tools.pywxrc
# Purpose:     XML resource compiler
#
# Author:      Robin Dunn
#              Based on wxrc.cpp by Vaclav Slavik, Eduardo Marques
#              Ported to Python in order to not require yet another
#              binary in wxPython distributions
#
# RCS-ID:      $Id$
# Copyright:   (c) 2004 by Total Control Software, 2000 Vaclav Slavik
# Licence:     wxWindows license
#----------------------------------------------------------------------

"""
pywxrc -- XML resource compiler

Usage: wxrc [-h] [-v] [-e] [-c] [-p] [-g] [-n <str>] [-o <str>] input file(s)...
  -h, --help            show help message
  -v, --verbose         be verbose
  -e, --extra-cpp-code  output C++ header file with XRC derived classes
  -c, --cpp-code        output C++ source rather than .xrs file
  -p, --python-code     output wxPython source rather than .rsc file
  -g, --gettext         output list of translatable strings (to stdout or file if -o used)
  -n, --function str    C++/Python function name (with -c or -p) [InitXmlResource]
  -o, --output str      output file [resource.xrs/cpp/py]
"""

import sys, os, getopt, glob
import wx
import wx.xrc


#----------------------------------------------------------------------

class XRCWidgetData:
    def __init__(self, vname, vclass):
        self.name = vname
        self.klass = vclass
    def GetName(self):
        return self.name
    def GetClass(self):
        return self.klass


#----------------------------------------------------------------------

class XRCWndClassData:
    def __init__(self, className, parentClassName, node):
        self.className = className
        self.parentClassName = parentClassName
        self.BrowseXmlNode(node.GetChildren())
        self.wdata = []


    def BrowseXmlNode(self, node):
        while node:
            if node.GetName() == "object" and node.HasProp("class") and node.HasProp("name"):
                classVal = node.GetPropVal("class", "")
                nameVal  = node.GetPropVal("name", "")
                self.wdata.append(XRCWidgetData(nameVal, classVal))
            children = node.GetChildren()
            if children:
                    self.BrowseXmlNode(children)
            node = node.GetNext()

            
    def GetWidgetData(self):
        return self.wdata

    
    def IsRealClass(self, name):
        if name in ['tool', 'unknown', 'notebookpage', 'separator',
                    'sizeritem', 'wxMenuItem']:
            return False
        else:
            return True


    def GenerateHeaderCode(self, file):
        file.write("class %s : public %s {\nprotected:\n" % (self.className, self.parentClassName))

        for w in self.wdata:
            if not self.IsRealClass(w.GetClass()):
                continue
            if not w.GetName():
                continue
            file.write(" " + w.GetClass() + "* " + w.GetName() + ";\n")
        
        file.write("\nprivate:\n void InitWidgetsFromXRC(){\n",
                   + "  wxXmlResource::Get()->LoadObject(this,NULL,\""
                   +  self.className
                   +  "\",\""
                   +  self.parentClassName
                   +  "\");\n");
            
        for w in self.wdata:
            if not self.IsRealClass(w.GetClass()):
                continue
            if not w.GetName():
                continue
            file.write( "  "
                        + w.GetName()
                        + " = XRCCTRL(*this,\""
                        + w.GetName()
                        + "\","
                        + w.GetClass()
                        + ");\n")
 
        file.write(" }\n")
        file.write("public:\n"
                   + self.className
                   + "::"
                   + self.className
                   + "(){\n"
                   + "  InitWidgetsFromXRC();\n"
                   + " }\n"
                   + "};\n")
                


#----------------------------------------------------------------------


class XmlResApp:
    def __init__(self):
        self.flagVerbose = False
        self.flagCPP = False
        self.flagH = False
        self.flagPython = False
        self.flagGettext = False
        self.parOutput = ""
        self.parFuncname = "InitXmlResource"
        self.parFiles = []
        self.aXRCWndClassData = []
        

    #--------------------------------------------------
    def main(self, args):
        try:
            opts, args = getopt.getopt(args, "hvecpgn:o:",
                 "help verbose extra-cpp-code cpp-code python-code gettext function= output=".split())
        except getopt.GetoptError:
            print __doc__
            sys.exit(1)

        for opt, val in opts:
            if opt in ["-h", "--help"]:
                print __doc__
                sys.exit(1)

            if opt in ["-v", "--verbose"]:
                self.flagVerbose = True

            if opt in ["-e", "--extra-cpp-code"]:
                self.flagH = True

            if opt in ["-c", "--cpp-code"]:
                self.flagCPP = True

            if opt in ["-p", "--python-code"]:
                self.flagPython = True

            if opt in ["-g", "--gettext"]:
                self.flagGettext = True

            if opt in ["-n", "--function"]:
                self.parFuncname = val

            if opt in ["-o", "--output"]:
                self.parOutput = val

        if self.flagCPP + self.flagPython + self.flagGettext == 0:
            print __doc__
            print "\nYou must specify one of -c, -p or -g!\n"
            sys.exit(1)

        if self.flagCPP + self.flagPython + self.flagGettext > 1:
            print __doc__
            print "\n-c, -p and -g are mutually exclusive, specify only 1!\n"
            sys.exit(1)
            

        if self.parOutput:
            self.parOutput = os.path.normpath(self.parOutput)
            self.parOutputPath = os.path.split(self.parOutput)[0]
        else:
            self.parOutputPath = "."
            if self.flagCPP:
                self.parOutput = "resource.cpp"
            elif self.flagPython:
                self.parOutput = "resource.py"
            elif self.flagGettext:
                self.parOutput = ""
            else:
                self.parOutput = "resource.xrs"

        if not args:
            print __doc__
            sys.exit(1)
        for arg in args:
            self.parFiles += glob.glob(arg)

        self.retCode = 0
        if self.flagGettext:
            self.OutputGettext()
        else:
            self.CompileRes()



    #--------------------------------------------------
    def CompileRes(self):
        files = self.PrepareTempFiles()
        try:
            os.unlink(self.parOutput)
        except OSError:
            pass

        if not self.retCode:
            if self.flagCPP:
                self.MakePackageCPP(files)
                if self.flagH:
                    self.GenCPPHeader()

            elif self.flagPython:
                self.MakePackagePython(files)

            else:
                self.MakePackageZIP(files)

        self.DeleteTempFiles(files)


    #--------------------------------------------------
    def OutputGettext(self):
        pass
    

    #--------------------------------------------------
    def GetInternalFileName(self, name, flist):
        name2 = name;
        name2 = name2.replace(":", "_")
        name2 = name2.replace("/", "_")
        name2 = name2.replace("\\", "_")
        name2 = name2.replace("*", "_")
        name2 = name2.replace("?", "_")

        s = os.path.split(self.parOutput)[1] + "$" + name2

        if os.path.exists(s) and s not in flist:
            i = 0
            while True:
                s = os.path.split(self.parOutput)[1] + ("$%s%03d" % (name2, i))
                if not os.path.exists(s) or s in flist:
                    break
        return s;
   

    #--------------------------------------------------
    def PrepareTempFiles(self):
        flist = []
        for f in self.parFiles:
            if self.flagVerbose:
                print "processing %s..." % f

            doc = wx.xrc.EmptyXmlDocument()
            
            if not doc.Load(f):
                print "Error parsing file", f
                self.retCode = 1
                continue

            path, name = os.path.split(f)
            name, ext = os.path.splitext(name)

            self.FindFilesInXML(doc.GetRoot(), flist, path)
            if self.flagH:
                node = doc.GetRoot().GetChildren()
                while node:
                    if node.GetName() == "object" and node.HasProp("class") and node.HasProp("name"):
                        classVal = node.GetPropVal("class", "")
                        nameVal  = node.GetPropVal("name", "")
                        self.aXRCWndClassData.append(XRCWidgetData(nameVal, classVal))
                    node = node.GetNext()
            internalName = self.GetInternalFileName(f, flist)

            doc.Save(os.path.join(self.parOutputPath, internalName))
            flist.append(internalName)

        return flist
    
    
    #--------------------------------------------------
    # Does 'node' contain filename information at all?
    def NodeContainsFilename(self, node):
        # Any bitmaps:
        if node.GetName() == "bitmap":
            return True

        # URLs in wxHtmlWindow:
        if node.GetName() == "url":
            return True

        # wxBitmapButton:
        parent = node.GetParent()
        if parent != None and \
           parent.GetPropVal("class", "") == "wxBitmapButton" and \
           (node.GetName() == "focus" or node.etName() == "disabled" or
            node.GetName() == "selected"):
            return True

        # wxBitmap or wxIcon toplevel resources:
        if node.GetName() == "object":
            klass = node.GetPropVal("class", "")
            if klass == "wxBitmap" or klass == "wxIcon":
                return True

        return False

    #--------------------------------------------------
    # find all files mentioned in structure, e.g. <bitmap>filename</bitmap>
    def FindFilesInXML(self, node, flist, inputPath):
        # Is 'node' XML node element?
        if node is None: return
        if node.GetType() != wx.xrc.XML_ELEMENT_NODE: return

        containsFilename = self.NodeContainsFilename(node);

        n = node.GetChildren()
        while n:
            if (containsFilename and
                (n.GetType() == wx.xrc.XML_TEXT_NODE or
                 n.GetType() == wx.xrc.XML_CDATA_SECTION_NODE)):
                
                if os.path.isabs(n.GetContent()) or inputPath == "":
                    fullname = n.GetContent()
                else:
                    fullname = os.path.join(inputPath, n.GetContent())

                if self.flagVerbose:
                    print "adding     %s..." % fullname

                filename = self.GetInternalFileName(n.GetContent(), flist)
                n.SetContent(filename)

                if filename not in flist:
                    flist.append(filename)
                    
                inp = open(fullname)
                out = open(os.path.join(self.parOutputPath, filename), "w")
                out.write(inp.read())

            # subnodes:
            if n.GetType() == wx.xrc.XML_ELEMENT_NODE:
                self.FindFilesInXML(n, flist, inputPath);

            n = n.GetNext()
    


    #--------------------------------------------------
    def DeleteTempFiles(self, flist):
        for f in flist:
            os.unlink(os.path.join(self.parOutputPath, f))


    #--------------------------------------------------
    def MakePackageZIP(self, flist):
        files = " ".join(flist)

        if self.flagVerbose:
            print "compressing %s..." % self.parOutput

        cwd = os.getcwd()
        os.chdir(self.parOutputPath)
        cmd = "zip -9 -j "
        if not self.flagVerbose:
            cmd += "-q "
        cmd += self.parOutput + " " + files

        from distutils.spawn import spawn
        try:
            spawn(cmd.split())
            success = True
        except:
            success = False
            
        os.chdir(cwd)
    
        if not success:
            print "Unable to execute zip program. Make sure it is in the path."
            print "You can download it at http://www.cdrom.com/pub/infozip/"
            self.retCode = 1
    

    #--------------------------------------------------
    def FileToCppArray(self, filename, num):
        output = []
        buffer = open(filename, "rb").read()
        lng = len(buffer)

        output.append("static size_t xml_res_size_%d = %d;\n" % (num, lng))
        output.append("static unsigned char xml_res_file_%d[] = {\n" % num)
        # we cannot use string literals because MSVC is dumb wannabe compiler
        # with arbitrary limitation to 2048 strings :(

        linelng = 0
        for i in xrange(lng):
            tmp = "%i" % ord(buffer[i])
            if i != 0: output.append(',')
            if linelng > 70:
                linelng = 0
                output.append("\n")
        
            output.append(tmp)
            linelng += len(tmp)+1

        output.append("};\n\n")

        return "".join(output)


    
    #--------------------------------------------------
    def MakePackageCPP(self, flist):
        file = open(self.parOutput, "wt")

        if self.flagVerbose:
            print "creating C++ source file %s..." % self.parOutput

        file.write("""\
//
// This file was automatically generated by wxrc, do not edit by hand.
//

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/filesys.h>
#include <wx/fs_mem.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_all.h>

""")

        num = 0
        for f in flist:
            file.write(self.FileToCppArray(os.path.join(self.parOutputPath, f), num))
            num += 1
              

        file.write("void " + self.parFuncname + "()\n")
        file.write("""\
{

    // Check for memory FS. If not present, load the handler:
    {
        wxMemoryFSHandler::AddFile(wxT(\"XRC_resource/dummy_file\"), wxT(\"dummy one\"));
        wxFileSystem fsys;
        wxFSFile *f = fsys.OpenFile(wxT(\"memory:XRC_resource/dummy_file\"));
        wxMemoryFSHandler::RemoveFile(wxT(\"XRC_resource/dummy_file\"));
        if (f) delete f;
        else wxFileSystem::AddHandler(new wxMemoryFSHandler);
    }
""");

        for i in range(len(flist)):
            file.write("    wxMemoryFSHandler::AddFile(wxT(\"XRC_resource/" + flist[i])
            file.write("\"), xml_res_file_%i, xml_res_size_%i);\n" %(i, i))


        for i in range(len(self.parFiles)):
            file.write("    wxXmlResource::Get()->Load(wxT(\"memory:XRC_resource/" +
                       self.GetInternalFileName(self.parFiles[i], flist) +
                       "\"));\n")
    
        file.write("}\n")
        
    
    #--------------------------------------------------
    def GenCPPHeader(self):
        path, name = os.path.split(self.parOutput)
        name, ext = os.path.splitext(name)
        heaFileName = name+'.h'

        file = open(heaFileName, "wt")
        file.write("""\
//
// This file was automatically generated by wxrc, do not edit by hand.
//
""");
        file.write("#ifndef __"  + name + "_h__\n")
        file.write("#define __"  + name + "_h__\n")

        for data in self.aXRCWndClassData:
            data.GenerateHeaderCode(file)

        file.write("\nvoid \n" + self.parFuncname + "();\n#endif\n")
        

    #--------------------------------------------------
    def FileToPythonArray(self, filename, num):
        output = []
        buffer = open(filename, "rb").read()
        lng = len(buffer)

        output.append("    xml_res_file_%d = '''\\\n" % num)

        linelng = 0
        for i in xrange(lng):
            s = buffer[i]
            c = ord(s)
            if s == '\n':
                tmp = s
                linelng = 0
            elif c < 32 or c > 127 or s == "'":
                tmp = "\\x%02x" % c
            elif s == "\\":
                tmp = "\\\\"            
            else:
                tmp = s

            if linelng > 70:
                linelng = 0
                output.append("\\\n")

            output.append(tmp)
            linelng += len(tmp)

        output.append("'''\n\n")

        return "".join(output)

    #--------------------------------------------------
    def MakePackagePython(self, flist):
        file = open(self.parOutput, "wt")

        if self.flagVerbose:
            print "creating Python source file %s..." % self.parOutput
        
        file.write("""\
#
# This file was automatically generated by wxrc, do not edit by hand.
#

import wx
import wx.xrc

""")
        file.write("def " + self.parFuncname + "():\n")
        
        num = 0
        for f in flist:
            file.write(self.FileToPythonArray(os.path.join(self.parOutputPath, f), num))
            num += 1

        file.write("""
        
    # check if the memory filesystem handler has been loaded yet, and load it if not
    wx.MemoryFSHandler.AddFile('XRC_resource/dummy_file', 'dummy value')
    fsys = wx.FileSystem()
    f = fsys.OpenFile('memory:XRC_resource/dummy_file')
    wx.MemoryFSHandler.RemoveFile('XRC_resource/dummy_file')
    if f is not None:
        f.Destroy()
    else:
        wx.FileSystem.AddHandler(wx.MemoryFSHandler())

    # load all the strings as memory files and load into XmlRes
""")
        
        for i in range(len(flist)):
            file.write("    wx.MemoryFSHandler.AddFile('XRC_resource/" + flist[i] +
                       "', xml_res_file_%i)\n" % i)

        for pf in self.parFiles:
            file.write("    wx.xrc.XmlResource.Get().Load('memory:XRC_resource/" +
                       self.GetInternalFileName(pf, flist) + "')\n")
            
        
    #--------------------------------------------------
    def OutputGettext(self):
        strings = self.FindStrings()

        if not self.parOutput:
            out = sys.stdout
        else:
            out = open(self.parOutput, "wt")

        for st in strings:
            out.write("_(\"%s\")\n" % st)
            

        
    #--------------------------------------------------
    def FindStrings(self):
        strings = []
        for pf in self.parFiles:
            if self.flagVerbose:
                print "processing %s..." % pf

            doc = wx.xrc.EmptyXmlDocument()
            if not doc.Load(pf):
                print "Error parsing file", pf
                self.retCode = 1
                continue

            strings += self.FindStringsInNode(doc.GetRoot())

        return strings
            
   
    #--------------------------------------------------
    def ConvertText(self, st):
        st2 = ""
        dt = list(st)

        skipNext = False
        for i in range(len(dt)):
            if skipNext:
                skipNext = False
                continue
            
            if dt[i] == '_':
                if dt[i+1] == '_':
                    st2 += '_'
                    skipNext = True
                else:
                    st2 += '&'
            elif dt[i] == '\n':
                st2 += '\\n'
            elif dt[i] == '\t':
                st2 += '\\t'
            elif dt[i] == '\r':
                st2 += '\\r'
            elif dt[i] == '\\':
                if dt[i+1] not in ['n', 't', 'r']:
                    st2 += '\\\\'
                else:
                    st2 += '\\'
            elif dt[i] == '"':
                st2 += '\\"'
            else:            
                st2 += dt[i]

        return st2                
             
                
    
    #--------------------------------------------------
    def FindStringsInNode(self, parent):
        def is_number(st):
            try:
                i = int(st)
                return True
            except ValueError:
                return False
            
        strings = []
        if parent is None:
            return strings;
        child = parent.GetChildren()

        while child:
            if ((parent.GetType() == wx.xrc.XML_ELEMENT_NODE) and
                # parent is an element, i.e. has subnodes...
                (child.GetType() == wx.xrc.XML_TEXT_NODE or
                child.GetType() == wx.xrc.XML_CDATA_SECTION_NODE) and
                # ...it is textnode...
                (
                    parent.GetName() == "label" or
                    (parent.GetName() == "value" and
                                   not is_number(child.GetContent())) or
                    parent.GetName() == "help" or
                    parent.GetName() == "longhelp" or
                    parent.GetName() == "tooltip" or
                    parent.GetName() == "htmlcode" or
                    parent.GetName() == "title" or
                    parent.GetName() == "item"
                )):
                # ...and known to contain translatable string
                if (not self.flagGettext or
                    parent.GetPropVal("translate", "1") != "0"):

                    strings.append(self.ConvertText(child.GetContent()))

            # subnodes:
            if child.GetType() == wx.xrc.XML_ELEMENT_NODE:
                strings += self.FindStringsInNode(child)

            child = child.GetNext()

        return strings

#---------------------------------------------------------------------------

def main():
    XmlResApp().main(sys.argv[1:])


if __name__ == "__main__":
    main()

