/////////////////////////////////////////////////////////////////////////////
// Name:        wxrc.cpp
// Purpose:     XML resource compiler
// Author:      Vaclav Slavik, Eduardo Marques <edrdo@netcabo.pt>
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// To fix compilation when using Sun CC we must define wxT() for this file.
#define wxNEEDS_T

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
#endif

#include "wx/cmdline.h"
#include "wx/xml/xml.h"
#include "wx/ffile.h"
#include "wx/filename.h"
#include "wx/wfstream.h"
#include "wx/utils.h"
#include "wx/hashset.h"
#include "wx/mimetype.h"

WX_DECLARE_HASH_SET(wxString, wxStringHash, wxStringEqual, StringSet);

class XRCWidgetData
{
public:
    XRCWidgetData(const wxString& vname,const wxString& vclass)
        : m_class(vclass), m_name(vname)  {}
    const wxString& GetName() const { return m_name; }
    const wxString& GetClass() const { return m_class; }
private:
    wxString m_class;
    wxString m_name;
};

#include "wx/arrimpl.cpp"
WX_DECLARE_OBJARRAY(XRCWidgetData,ArrayOfXRCWidgetData);
WX_DEFINE_OBJARRAY(ArrayOfXRCWidgetData)

class XRCWndClassData
{
private:
    wxString m_className;
    wxString m_parentClassName;
    StringSet m_ancestorClassNames;
    ArrayOfXRCWidgetData m_wdata;

    void BrowseXmlNode(wxXmlNode* node)
    {
        wxString classValue;
        wxString nameValue;
        wxXmlNode* children;
        while (node)
        {
            if (node->GetName() == wxT("object")
                && node->GetPropVal(wxT("class"),&classValue)
                && node->GetPropVal(wxT("name"),&nameValue))
            {
                m_wdata.Add(XRCWidgetData(nameValue,classValue));
            }
            children = node->GetChildren();
            if (children)
                    BrowseXmlNode(children);
            node = node->GetNext();
        }
    }

public:
    XRCWndClassData(const wxString& className,
                    const wxString& parentClassName,
                    const wxXmlNode* node) :
        m_className(className) , m_parentClassName(parentClassName)
    {
        if ( className == wxT("wxMenu") )
        {
            m_ancestorClassNames.insert(wxT("wxMenu"));
            m_ancestorClassNames.insert(wxT("wxMenuBar"));
        }
        else if ( className == wxT("wxMDIChildFrame") )
        {
            m_ancestorClassNames.insert(wxT("wxMDIParentFrame"));
        }
        else if( className == wxT("wxMenuBar") ||
                    className == wxT("wxStatusBar") ||
                        className == wxT("wxToolBar") )
        {
            m_ancestorClassNames.insert(wxT("wxFrame"));
        }
        else
        {
            m_ancestorClassNames.insert(wxT("wxWindow"));
        }

        BrowseXmlNode(node->GetChildren());
    }

    const ArrayOfXRCWidgetData& GetWidgetData()
    {
        return m_wdata;
    }

    bool IsRealClass(const wxString& name)
    {
        if (name == wxT("tool") ||
            name == wxT("data") ||
            name == wxT("unknown") ||
            name == wxT("notebookpage") ||
            name == wxT("separator") ||
            name == wxT("sizeritem") ||
            name == wxT("wxMenu") ||
            name == wxT("wxMenuBar") ||
            name == wxT("wxMenuItem") ||
            name.EndsWith(wxT("Sizer")) )
        {
            return false;
        }
        return true;
    }

    void GenerateHeaderCode(wxFFile& file)
    {

        file.Write(wxT("class ") + m_className + wxT(" : public ") + m_parentClassName
                   + wxT(" {\nprotected:\n"));
        size_t i;
        for(i=0;i<m_wdata.Count();++i)
        {
            const XRCWidgetData& w = m_wdata.Item(i);
            if( !IsRealClass(w.GetClass()) ) continue;
            if( w.GetName().Length() == 0 ) continue;
            file.Write(
                wxT(" ") + w.GetClass() + wxT("* ") + w.GetName()
                + wxT(";\n"));
        }
        file.Write(wxT("\nprivate:\n void InitWidgetsFromXRC(wxWindow *parent){\n")
                   wxT("  wxXmlResource::Get()->LoadObject(this,parent,wxT(\"")
                   +  m_className
                   +  wxT("\"), wxT(\"")
                   +  m_parentClassName
                   +  wxT("\"));\n"));
        for(i=0;i<m_wdata.Count();++i)
        {
            const XRCWidgetData& w = m_wdata.Item(i);
            if( !IsRealClass(w.GetClass()) ) continue;
            if( w.GetName().Length() == 0 ) continue;
            file.Write( wxT("  ")
                        + w.GetName()
                        + wxT(" = XRCCTRL(*this,\"")
                        + w.GetName()
                        + wxT("\",")
                        + w.GetClass()
                        + wxT(");\n"));
        }
        file.Write(wxT(" }\n"));

        file.Write( wxT("public:\n"));

        if ( m_ancestorClassNames.size() == 1 )
        {
            file.Write
                 (
                    m_className +
                    wxT("(") +
                        *m_ancestorClassNames.begin() +
                        wxT(" *parent=NULL){\n") +
                    wxT("  InitWidgetsFromXRC((wxWindow *)parent);\n")
                    wxT(" }\n")
                    wxT("};\n")
                 );
        }
        else
        {
            file.Write(m_className + wxT("(){\n") +
                       wxT("  InitWidgetsFromXRC(NULL);\n")
                       wxT(" }\n")
                       wxT("};\n"));

            for ( StringSet::const_iterator it = m_ancestorClassNames.begin();
                  it != m_ancestorClassNames.end();
                  ++it )
            {
                file.Write(m_className + wxT("(") + *it + wxT(" *parent){\n") +
                            wxT("  InitWidgetsFromXRC((wxWindow *)parent);\n")
                            wxT(" }\n")
                            wxT("};\n"));
            }
        }
    }
};
WX_DECLARE_OBJARRAY(XRCWndClassData,ArrayOfXRCWndClassData);
WX_DEFINE_OBJARRAY(ArrayOfXRCWndClassData)


class XmlResApp : public wxAppConsole
{
public:
    // don't use builtin cmd line parsing:
    virtual bool OnInit() { return true; }
    virtual int OnRun();

private:
    void ParseParams(const wxCmdLineParser& cmdline);
    void CompileRes();
    wxArrayString PrepareTempFiles();
    void FindFilesInXML(wxXmlNode *node, wxArrayString& flist, const wxString& inputPath);

    wxString GetInternalFileName(const wxString& name, const wxArrayString& flist);
    void DeleteTempFiles(const wxArrayString& flist);
    void MakePackageZIP(const wxArrayString& flist);
    void MakePackageCPP(const wxArrayString& flist);
    void MakePackagePython(const wxArrayString& flist);

    void OutputGettext();
    wxArrayString FindStrings();
    wxArrayString FindStrings(wxXmlNode *node);

    bool flagVerbose, flagCPP, flagPython, flagGettext;
    wxString parOutput, parFuncname, parOutputPath;
    wxArrayString parFiles;
    int retCode;

    ArrayOfXRCWndClassData aXRCWndClassData;
        bool flagH;
        void GenCPPHeader();
};

IMPLEMENT_APP_CONSOLE(XmlResApp)

int XmlResApp::OnRun()
{
    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_SWITCH, wxT("h"), wxT("help"),  wxT("show help message"),
              wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, wxT("v"), wxT("verbose"), wxT("be verbose"), (wxCmdLineParamType)0, 0 },
        { wxCMD_LINE_SWITCH, wxT("e"), wxT("extra-cpp-code"),  wxT("output C++ header file with XRC derived classes"), (wxCmdLineParamType)0, 0 },
        { wxCMD_LINE_SWITCH, wxT("c"), wxT("cpp-code"),  wxT("output C++ source rather than .rsc file"), (wxCmdLineParamType)0, 0 },
        { wxCMD_LINE_SWITCH, wxT("p"), wxT("python-code"),  wxT("output wxPython source rather than .rsc file"), (wxCmdLineParamType)0, 0 },
        { wxCMD_LINE_SWITCH, wxT("g"), wxT("gettext"),  wxT("output list of translatable strings (to stdout or file if -o used)"), (wxCmdLineParamType)0, 0 },
        { wxCMD_LINE_OPTION, wxT("n"), wxT("function"),  wxT("C++/Python function name (with -c or -p) [InitXmlResource]"), (wxCmdLineParamType)0, 0 },
        { wxCMD_LINE_OPTION, wxT("o"), wxT("output"),  wxT("output file [resource.xrs/cpp]"), (wxCmdLineParamType)0, 0 },
#if 0 // not yet implemented
        { wxCMD_LINE_OPTION, wxT("l"), wxT("list-of-handlers"),  wxT("output list of necessary handlers to this file"), (wxCmdLineParamType)0, 0 },
#endif
        { wxCMD_LINE_PARAM,  NULL, NULL, wxT("input file(s)"),
              wxCMD_LINE_VAL_STRING,
              wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_OPTION_MANDATORY },

        { wxCMD_LINE_NONE, NULL, NULL, NULL, (wxCmdLineParamType)0, 0 }
    };

    wxCmdLineParser parser(cmdLineDesc, argc, argv);

    switch (parser.Parse())
    {
        case -1:
            return 0;

        case 0:
            retCode = 0;
            ParseParams(parser);
            if (flagGettext)
                OutputGettext();
            else
                CompileRes();
            return retCode;
    }
    return 1;
}




void XmlResApp::ParseParams(const wxCmdLineParser& cmdline)
{
    flagGettext = cmdline.Found(wxT("g"));
    flagVerbose = cmdline.Found(wxT("v"));
    flagCPP = cmdline.Found(wxT("c"));
    flagPython = cmdline.Found(wxT("p"));
    flagH = flagCPP && cmdline.Found(wxT("e"));


    if (!cmdline.Found(wxT("o"), &parOutput))
    {
        if (flagGettext)
            parOutput = wxEmptyString;
        else
        {
            if (flagCPP)
                parOutput = wxT("resource.cpp");
            else if (flagPython)
                parOutput = wxT("resource.py");
            else
                parOutput = wxT("resource.xrs");
        }
    }
    if (!parOutput.empty())
    {
        wxFileName fn(parOutput);
        fn.Normalize();
        parOutput = fn.GetFullPath();
        parOutputPath = wxPathOnly(parOutput);
    }
    if (!parOutputPath) parOutputPath = wxT(".");

    if (!cmdline.Found(wxT("n"), &parFuncname))
        parFuncname = wxT("InitXmlResource");

    for (size_t i = 0; i < cmdline.GetParamCount(); i++)
    {
#ifdef __WINDOWS__
        wxString fn=wxFindFirstFile(cmdline.GetParam(i), wxFILE);
        while (!fn.empty())
        {
            parFiles.Add(fn);
            fn=wxFindNextFile();
        }
#else
        parFiles.Add(cmdline.GetParam(i));
#endif
    }
}




void XmlResApp::CompileRes()
{
    wxArrayString files = PrepareTempFiles();

    wxRemoveFile(parOutput);

    if (!retCode)
    {
        if (flagCPP){
            MakePackageCPP(files);
            if (flagH)
                GenCPPHeader();
        }
        else if (flagPython)
            MakePackagePython(files);
        else
            MakePackageZIP(files);
    }

    DeleteTempFiles(files);
}


wxString XmlResApp::GetInternalFileName(const wxString& name, const wxArrayString& flist)
{
    wxString name2 = name;
    name2.Replace(wxT(":"), wxT("_"));
    name2.Replace(wxT("/"), wxT("_"));
    name2.Replace(wxT("\\"), wxT("_"));
    name2.Replace(wxT("*"), wxT("_"));
    name2.Replace(wxT("?"), wxT("_"));

    wxString s = wxFileNameFromPath(parOutput) + wxT("$") + name2;

    if (wxFileExists(s) && flist.Index(s) == wxNOT_FOUND)
    {
        for (int i = 0;; i++)
        {
            s.Printf(wxFileNameFromPath(parOutput) + wxT("$%03i-") + name2, i);
            if (!wxFileExists(s) || flist.Index(s) != wxNOT_FOUND)
                break;
        }
    }
    return s;
}

wxArrayString XmlResApp::PrepareTempFiles()
{
    wxArrayString flist;

    for (size_t i = 0; i < parFiles.Count(); i++)
    {
        if (flagVerbose)
            wxPrintf(wxT("processing ") + parFiles[i] +  wxT("...\n"));

        wxXmlDocument doc;

        if (!doc.Load(parFiles[i]))
        {
            wxLogError(wxT("Error parsing file ") + parFiles[i]);
            retCode = 1;
            continue;
        }

        wxString name, ext, path;
        wxSplitPath(parFiles[i], &path, &name, &ext);

        FindFilesInXML(doc.GetRoot(), flist, path);
        if (flagH)
        {
            wxXmlNode* node = (doc.GetRoot())->GetChildren();
                wxString classValue,nameValue;
                while(node){
                    if(node->GetName() == wxT("object")
                     && node->GetPropVal(wxT("class"),&classValue)
                     && node->GetPropVal(wxT("name"),&nameValue)){

                      aXRCWndClassData.Add(
                        XRCWndClassData(nameValue,classValue,node)
                      );
                    }
                    node = node -> GetNext();
            }
        }
        wxString internalName = GetInternalFileName(parFiles[i], flist);

        doc.Save(parOutputPath + wxFILE_SEP_PATH + internalName);
        flist.Add(internalName);
    }

    return flist;
}


// Does 'node' contain filename information at all?
static bool NodeContainsFilename(wxXmlNode *node)
{
   const wxString name = node->GetName();

   // Any bitmaps (bitmap2 is used for disabled toolbar buttons):
   if ( name == wxT("bitmap") || name == wxT("bitmap2") )
       return true;

   if ( name == wxT("icon") )
       return true;

   // wxBitmapButton:
   wxXmlNode *parent = node->GetParent();
   if (parent != NULL &&
       parent->GetPropVal(wxT("class"), wxT("")) == wxT("wxBitmapButton") &&
       (name == wxT("focus") ||
        name == wxT("disabled") ||
        name == wxT("hover") ||
        name == wxT("selected")))
       return true;

   // wxBitmap or wxIcon toplevel resources:
   if ( name == wxT("object") )
   {
       wxString klass = node->GetPropVal(wxT("class"), wxEmptyString);
       if (klass == wxT("wxBitmap") || klass == wxT("wxIcon"))
           return true;
   }

   // URLs in wxHtmlWindow:
   if ( name == wxT("url") &&
        parent != NULL &&
        parent->GetPropVal(wxT("class"), wxT("")) == wxT("wxHtmlWindow") )
   {
       // FIXME: this is wrong for e.g. http:// URLs
       return true;
   }

   return false;
}

// find all files mentioned in structure, e.g. <bitmap>filename</bitmap>
void XmlResApp::FindFilesInXML(wxXmlNode *node, wxArrayString& flist, const wxString& inputPath)
{
    // Is 'node' XML node element?
    if (node == NULL) return;
    if (node->GetType() != wxXML_ELEMENT_NODE) return;

    bool containsFilename = NodeContainsFilename(node);

    wxXmlNode *n = node->GetChildren();
    while (n)
    {
        if (containsFilename &&
            (n->GetType() == wxXML_TEXT_NODE ||
             n->GetType() == wxXML_CDATA_SECTION_NODE))
        {
            wxString fullname;
            if (wxIsAbsolutePath(n->GetContent()) || inputPath.empty())
                fullname = n->GetContent();
            else
                fullname = inputPath + wxFILE_SEP_PATH + n->GetContent();

            if (flagVerbose)
                wxPrintf(wxT("adding     ") + fullname +  wxT("...\n"));

            wxString filename = GetInternalFileName(n->GetContent(), flist);
            n->SetContent(filename);

            if (flist.Index(filename) == wxNOT_FOUND)
                flist.Add(filename);

            wxFileInputStream sin(fullname);
            wxFileOutputStream sout(parOutputPath + wxFILE_SEP_PATH + filename);
            sin.Read(sout); // copy the stream
        }

        // subnodes:
        if (n->GetType() == wxXML_ELEMENT_NODE)
            FindFilesInXML(n, flist, inputPath);

        n = n->GetNext();
    }
}



void XmlResApp::DeleteTempFiles(const wxArrayString& flist)
{
    for (size_t i = 0; i < flist.Count(); i++)
        wxRemoveFile(parOutputPath + wxFILE_SEP_PATH + flist[i]);
}



void XmlResApp::MakePackageZIP(const wxArrayString& flist)
{
    wxString files;

    for (size_t i = 0; i < flist.Count(); i++)
        files += flist[i] + wxT(" ");
    files.RemoveLast();

    if (flagVerbose)
        wxPrintf(wxT("compressing ") + parOutput +  wxT("...\n"));

    wxString cwd = wxGetCwd();
    wxSetWorkingDirectory(parOutputPath);
    int execres = wxExecute(wxT("zip -9 -j ") +
                            wxString(flagVerbose ? wxT("\"") : wxT("-q \"")) +
                            parOutput + wxT("\" ") + files, true);
    wxSetWorkingDirectory(cwd);
    if (execres == -1)
    {
        wxLogError(wxT("Unable to execute zip program. Make sure it is in the path."));
        wxLogError(wxT("You can download it at http://www.cdrom.com/pub/infozip/"));
        retCode = 1;
        return;
    }
}



static wxString FileToCppArray(wxString filename, int num)
{
    wxString output;
    wxString tmp;
    wxString snum;
    wxFFile file(filename, wxT("rb"));
    wxFileOffset offset = file.Length();
    wxASSERT_MSG( offset >= 0 , wxT("Invalid file length") );

    const size_t lng = wx_truncate_cast(size_t, offset);
    wxASSERT_MSG( !(lng - offset), wxT("Huge file not supported") );

    snum.Printf(wxT("%i"), num);
    output.Printf(wxT("static size_t xml_res_size_") + snum + wxT(" = %i;\n"), lng);
    output += wxT("static unsigned char xml_res_file_") + snum + wxT("[] = {\n");
    // we cannot use string literals because MSVC is dumb wannabe compiler
    // with arbitrary limitation to 2048 strings :(

    unsigned char *buffer = new unsigned char[lng];
    file.Read(buffer, lng);

    for (size_t i = 0, linelng = 0; i < lng; i++)
    {
        tmp.Printf(wxT("%i"), buffer[i]);
        if (i != 0) output << wxT(',');
        if (linelng > 70)
        {
            linelng = 0;
            output << wxT("\n");
        }
        output << tmp;
        linelng += tmp.Length()+1;
    }

    delete[] buffer;

    output += wxT("};\n\n");

    return output;
}


void XmlResApp::MakePackageCPP(const wxArrayString& flist)
{
    wxFFile file(parOutput, wxT("wt"));
    size_t i;

    if (flagVerbose)
        wxPrintf(wxT("creating C++ source file ") + parOutput +  wxT("...\n"));

    file.Write(wxT("")
wxT("//\n")
wxT("// This file was automatically generated by wxrc, do not edit by hand.\n")
wxT("//\n\n")
wxT("#include <wx/wxprec.h>\n")
wxT("\n")
wxT("#ifdef __BORLANDC__\n")
wxT("    #pragma hdrstop\n")
wxT("#endif\n")
wxT("\n")
wxT("")
wxT("#include <wx/filesys.h>\n")
wxT("#include <wx/fs_mem.h>\n")
wxT("#include <wx/xrc/xmlres.h>\n")
wxT("#include <wx/xrc/xh_all.h>\n")
wxT("\n")
wxT("#if wxCHECK_VERSION(2,8,5) && wxABI_VERSION >= 20805\n")
wxT("    #define XRC_ADD_FILE(name, data, size, mime) \\\n")
wxT("        wxMemoryFSHandler::AddFileWithMimeType(name, data, size, mime)\n")
wxT("#else\n")
wxT("    #define XRC_ADD_FILE(name, data, size, mime) \\\n")
wxT("        wxMemoryFSHandler::AddFile(name, data, size)\n")
wxT("#endif\n")
wxT("\n"));

    for (i = 0; i < flist.Count(); i++)
        file.Write(
              FileToCppArray(parOutputPath + wxFILE_SEP_PATH + flist[i], i));

    file.Write(wxT("")
wxT("void ") + parFuncname + wxT("()\n")
wxT("{\n")
wxT("\n")
wxT("    // Check for memory FS. If not present, load the handler:\n")
wxT("    {\n")
wxT("        wxMemoryFSHandler::AddFile(wxT(\"XRC_resource/dummy_file\"), wxT(\"dummy one\"));\n")
wxT("        wxFileSystem fsys;\n")
wxT("        wxFSFile *f = fsys.OpenFile(wxT(\"memory:XRC_resource/dummy_file\"));\n")
wxT("        wxMemoryFSHandler::RemoveFile(wxT(\"XRC_resource/dummy_file\"));\n")
wxT("        if (f) delete f;\n")
wxT("        else wxFileSystem::AddHandler(new wxMemoryFSHandler);\n")
wxT("    }\n")
wxT("\n"));

    for (i = 0; i < flist.Count(); i++)
    {
        wxString s;

        wxString mime;
        wxString ext = wxFileName(flist[i]).GetExt();
        if ( ext.Lower() == wxT("xrc") )
            mime = wxT("text/xml");
#if wxUSE_MIMETYPE
        else
        {
            wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);
            if ( ft )
            {
                ft->GetMimeType(&mime);
                delete ft;
            }
        }
#endif // wxUSE_MIMETYPE

        s.Printf(wxT("    XRC_ADD_FILE(wxT(\"XRC_resource/") + flist[i] +
                 wxT("\"), xml_res_file_%i, xml_res_size_%i, wxT(\"%s\"));\n"),
                 i, i, mime.c_str());
        file.Write(s);
    }

    for (i = 0; i < parFiles.Count(); i++)
    {
        file.Write(wxT("    wxXmlResource::Get()->Load(wxT(\"memory:XRC_resource/") +
                   GetInternalFileName(parFiles[i], flist) + wxT("\"));\n"));
    }

    file.Write(wxT("}\n"));


}

void XmlResApp::GenCPPHeader()
{
    wxString fileSpec = ((parOutput.BeforeLast('.')).AfterLast('/')).AfterLast('\\');
    wxString heaFileName = fileSpec + wxT(".h");

    wxFFile file(heaFileName, wxT("wt"));
    file.Write(
wxT("//\n")
wxT("// This file was automatically generated by wxrc, do not edit by hand.\n")
wxT("//\n\n")
wxT("#ifndef __")  + fileSpec + wxT("_h__\n")
wxT("#define __")  + fileSpec + wxT("_h__\n")
);
    for(size_t i=0;i<aXRCWndClassData.Count();++i){
                aXRCWndClassData.Item(i).GenerateHeaderCode(file);
    }
    file.Write(
                wxT("\nvoid \n")
                + parFuncname
                + wxT("();\n#endif\n"));
}

static wxString FileToPythonArray(wxString filename, int num)
{
    wxString output;
    wxString tmp;
    wxString snum;
    wxFFile file(filename, wxT("rb"));
    wxFileOffset offset = file.Length();
    wxASSERT_MSG( offset >= 0 , wxT("Invalid file length") );

    const size_t lng = wx_truncate_cast(size_t, offset);
    wxASSERT_MSG( !(offset - lng), wxT("Huge file not supported") );

    snum.Printf(wxT("%i"), num);
    output = wxT("    xml_res_file_") + snum + wxT(" = '''\\\n");

    unsigned char *buffer = new unsigned char[lng];
    file.Read(buffer, lng);

    for (size_t i = 0, linelng = 0; i < lng; i++)
    {
        unsigned char c = buffer[i];
        if (c == '\n')
        {
            tmp = (wxChar)c;
            linelng = 0;
        }
        else if (c < 32 || c > 127 || c == '\'')
            tmp.Printf(wxT("\\x%02x"), c);
        else if (c == '\\')
            tmp = wxT("\\\\");
        else
            tmp = (wxChar)c;
        if (linelng > 70)
        {
            linelng = 0;
            output << wxT("\\\n");
        }
        output << tmp;
        linelng += tmp.Length();
    }

    delete[] buffer;

    output += wxT("'''\n\n");

    return output;
}


void XmlResApp::MakePackagePython(const wxArrayString& flist)
{
    wxFFile file(parOutput, wxT("wt"));
    size_t i;

    if (flagVerbose)
        wxPrintf(wxT("creating Python source file ") + parOutput +  wxT("...\n"));

    file.Write(
       wxT("#\n")
       wxT("# This file was automatically generated by wxrc, do not edit by hand.\n")
       wxT("#\n\n")
       wxT("import wx\n")
       wxT("import wx.xrc\n\n")
    );


    file.Write(wxT("def ") + parFuncname + wxT("():\n"));

    for (i = 0; i < flist.Count(); i++)
        file.Write(
          FileToPythonArray(parOutputPath + wxFILE_SEP_PATH + flist[i], i));

    file.Write(
        wxT("    # check if the memory filesystem handler has been loaded yet, and load it if not\n")
        wxT("    wx.MemoryFSHandler.AddFile('XRC_resource/dummy_file', 'dummy value')\n")
        wxT("    fsys = wx.FileSystem()\n")
        wxT("    f = fsys.OpenFile('memory:XRC_resource/dummy_file')\n")
        wxT("    wx.MemoryFSHandler.RemoveFile('XRC_resource/dummy_file')\n")
        wxT("    if f is not None:\n")
        wxT("        f.Destroy()\n")
        wxT("    else:\n")
        wxT("        wx.FileSystem.AddHandler(wx.MemoryFSHandler())\n")
        wxT("\n")
        wxT("    # load all the strings as memory files and load into XmlRes\n")
        );


    for (i = 0; i < flist.Count(); i++)
    {
        wxString s;
        s.Printf(wxT("    wx.MemoryFSHandler.AddFile('XRC_resource/") + flist[i] +
                 wxT("', xml_res_file_%i)\n"), i);
        file.Write(s);
    }
    for (i = 0; i < parFiles.Count(); i++)
    {
        file.Write(wxT("    wx.xrc.XmlResource.Get().Load('memory:XRC_resource/") +
                   GetInternalFileName(parFiles[i], flist) + wxT("')\n"));
    }

    file.Write(wxT("\n"));
}



void XmlResApp::OutputGettext()
{
    wxArrayString str = FindStrings();

    wxFFile fout;
    if (parOutput.empty())
        fout.Attach(stdout);
    else
        fout.Open(parOutput, wxT("wt"));

    for (size_t i = 0; i < str.GetCount(); i++)
        fout.Write(wxT("_(\"") + str[i] + wxT("\");\n"));

    if (!parOutput) fout.Detach();
}



wxArrayString XmlResApp::FindStrings()
{
    wxArrayString arr, a2;

    for (size_t i = 0; i < parFiles.Count(); i++)
    {
        if (flagVerbose)
            wxPrintf(wxT("processing ") + parFiles[i] +  wxT("...\n"));

        wxXmlDocument doc;
        if (!doc.Load(parFiles[i]))
        {
            wxLogError(wxT("Error parsing file ") + parFiles[i]);
            retCode = 1;
            continue;
        }
        a2 = FindStrings(doc.GetRoot());
        WX_APPEND_ARRAY(arr, a2);
    }

    return arr;
}



static wxString ConvertText(const wxString& str)
{
    wxString str2;
    const wxChar *dt;

    for (dt = str.c_str(); *dt; dt++)
    {
        if (*dt == wxT('_'))
        {
            if ( *(++dt) == wxT('_') )
                str2 << wxT('_');
            else
                str2 << wxT('&') << *dt;
        }
        else
        {
            switch (*dt)
            {
                case wxT('\n') : str2 << wxT("\\n"); break;
                case wxT('\t') : str2 << wxT("\\t"); break;
                case wxT('\r') : str2 << wxT("\\r"); break;
                case wxT('\\') : if ((*(dt+1) != 'n') &&
                                     (*(dt+1) != 't') &&
                                     (*(dt+1) != 'r'))
                                     str2 << wxT("\\\\");
                                 else
                                     str2 << wxT("\\");
                                 break;
                case wxT('"')  : str2 << wxT("\\\""); break;
                default        : str2 << *dt; break;
            }
        }
    }

    return str2;
}


wxArrayString XmlResApp::FindStrings(wxXmlNode *node)
{
    wxArrayString arr;

    wxXmlNode *n = node;
    if (n == NULL) return arr;
    n = n->GetChildren();

    while (n)
    {
        if ((node->GetType() == wxXML_ELEMENT_NODE) &&
            // parent is an element, i.e. has subnodes...
            (n->GetType() == wxXML_TEXT_NODE ||
            n->GetType() == wxXML_CDATA_SECTION_NODE) &&
            // ...it is textnode...
            (
                node/*not n!*/->GetName() == wxT("label") ||
                (node/*not n!*/->GetName() == wxT("value") &&
                               !n->GetContent().IsNumber()) ||
                node/*not n!*/->GetName() == wxT("help") ||
                node/*not n!*/->GetName() == wxT("longhelp") ||
                node/*not n!*/->GetName() == wxT("tooltip") ||
                node/*not n!*/->GetName() == wxT("htmlcode") ||
                node/*not n!*/->GetName() == wxT("title") ||
                node/*not n!*/->GetName() == wxT("item")
            ))
            // ...and known to contain translatable string
        {
            if (!flagGettext ||
                node->GetPropVal(wxT("translate"), wxT("1")) != wxT("0"))
            {
                arr.Add(ConvertText(n->GetContent()));
            }
        }

        // subnodes:
        if (n->GetType() == wxXML_ELEMENT_NODE)
        {
            wxArrayString a2 = FindStrings(n);
            WX_APPEND_ARRAY(arr, a2);
        }

        n = n->GetNext();
    }
    return arr;
}
