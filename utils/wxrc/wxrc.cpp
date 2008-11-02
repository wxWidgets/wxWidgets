/////////////////////////////////////////////////////////////////////////////
// Name:        wxrc.cpp
// Purpose:     XML resource compiler
// Author:      Vaclav Slavik, Eduardo Marques <edrdo@netcabo.pt>
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/wxcrtvararg.h"
#endif

#include "wx/cmdline.h"
#include "wx/xml/xml.h"
#include "wx/ffile.h"
#include "wx/filename.h"
#include "wx/wfstream.h"
#include "wx/utils.h"
#include "wx/hashset.h"
#include "wx/mimetype.h"
#include "wx/vector.h"

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
            if (node->GetName() == _T("object")
                && node->GetAttribute(_T("class"),&classValue)
                && node->GetAttribute(_T("name"),&nameValue))
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
        if ( className == _T("wxMenu") )
        {
            m_ancestorClassNames.insert(_T("wxMenu"));
            m_ancestorClassNames.insert(_T("wxMenuBar"));
        }
        else if ( className == _T("wxMDIChildFrame") )
        {
            m_ancestorClassNames.insert(_T("wxMDIParentFrame"));
        }
        else if( className == _T("wxMenuBar") ||
                    className == _T("wxStatusBar") ||
                        className == _T("wxToolBar") )
        {
            m_ancestorClassNames.insert(_T("wxFrame"));
        }
        else
        {
            m_ancestorClassNames.insert(_T("wxWindow"));
        }

        BrowseXmlNode(node->GetChildren());
    }

    const ArrayOfXRCWidgetData& GetWidgetData()
    {
        return m_wdata;
    }

    bool CanBeUsedWithXRCCTRL(const wxString& name)
    {
        if (name == _T("tool") ||
            name == _T("data") ||
            name == _T("unknown") ||
            name == _T("notebookpage") ||
            name == _T("separator") ||
            name == _T("sizeritem") ||
            name == _T("wxMenuBar") ||
            name == _T("wxMenuItem") ||
            name == _T("wxStaticBoxSizer") )
        {
            return false;
        }
        return true;
    }

    void GenerateHeaderCode(wxFFile& file)
    {

        file.Write(_T("class ") + m_className + _T(" : public ") + m_parentClassName
                   + _T(" {\nprotected:\n"));
        size_t i;
        for(i=0;i<m_wdata.GetCount();++i)
        {
            const XRCWidgetData& w = m_wdata.Item(i);
            if( !CanBeUsedWithXRCCTRL(w.GetClass()) ) continue;
            if( w.GetName().Length() == 0 ) continue;
            file.Write(
                _T(" ") + w.GetClass() + _T("* ") + w.GetName()
                + _T(";\n"));
        }
        file.Write(_T("\nprivate:\n void InitWidgetsFromXRC(wxWindow *parent){\n")
                   _T("  wxXmlResource::Get()->LoadObject(this,parent,_T(\"")
                   +  m_className
                   +  _T("\"), _T(\"")
                   +  m_parentClassName
                   +  _T("\"));\n"));
        for(i=0;i<m_wdata.GetCount();++i)
        {
            const XRCWidgetData& w = m_wdata.Item(i);
            if( !CanBeUsedWithXRCCTRL(w.GetClass()) ) continue;
            if( w.GetName().Length() == 0 ) continue;
            file.Write( _T("  ")
                        + w.GetName()
                        + _T(" = XRCCTRL(*this,\"")
                        + w.GetName()
                        + _T("\",")
                        + w.GetClass()
                        + _T(");\n"));
        }
        file.Write(_T(" }\n"));

        file.Write( _T("public:\n"));

        if ( m_ancestorClassNames.size() == 1 )
        {
            file.Write
                 (
                    m_className +
                    _T("(") +
                        *m_ancestorClassNames.begin() +
                        _T(" *parent=NULL){\n") +
                    _T("  InitWidgetsFromXRC((wxWindow *)parent);\n")
                    _T(" }\n")
                    _T("};\n")
                 );
        }
        else
        {
            file.Write(m_className + _T("(){\n") +
                       _T("  InitWidgetsFromXRC(NULL);\n")
                       _T(" }\n")
                       _T("};\n"));

            for ( StringSet::const_iterator it = m_ancestorClassNames.begin();
                  it != m_ancestorClassNames.end();
                  ++it )
            {
                file.Write(m_className + _T("(") + *it + _T(" *parent){\n") +
                            _T("  InitWidgetsFromXRC((wxWindow *)parent);\n")
                            _T(" }\n")
                            _T("};\n"));
            }
        }
    }
};
WX_DECLARE_OBJARRAY(XRCWndClassData,ArrayOfXRCWndClassData);
WX_DEFINE_OBJARRAY(ArrayOfXRCWndClassData)

struct ExtractedString
{
    ExtractedString() : lineNo(-1) {}
    ExtractedString(const wxString& str_,
                    const wxString& filename_, int lineNo_)
        : str(str_), filename(filename_), lineNo(lineNo_)
    {}

    wxString str;

    wxString filename;
    int      lineNo;
};

typedef wxVector<ExtractedString> ExtractedStrings;


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
    ExtractedStrings FindStrings();
    ExtractedStrings FindStrings(const wxString& filename, wxXmlNode *node);

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
        { wxCMD_LINE_SWITCH, "h", "help",  "show help message", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, "v", "verbose", "be verbose" },
        { wxCMD_LINE_SWITCH, "e", "extra-cpp-code",  "output C++ header file with XRC derived classes" },
        { wxCMD_LINE_SWITCH, "c", "cpp-code",  "output C++ source rather than .rsc file" },
        { wxCMD_LINE_SWITCH, "p", "python-code",  "output wxPython source rather than .rsc file" },
        { wxCMD_LINE_SWITCH, "g", "gettext",  "output list of translatable strings (to stdout or file if -o used)" },
        { wxCMD_LINE_OPTION, "n", "function",  "C++/Python function name (with -c or -p) [InitXmlResource]" },
        { wxCMD_LINE_OPTION, "o", "output",  "output file [resource.xrs/cpp]" },
#if 0 // not yet implemented
        { wxCMD_LINE_OPTION, "l", "list-of-handlers",  "output list of necessary handlers to this file" },
#endif
        { wxCMD_LINE_PARAM,  NULL, NULL, "input file(s)",
              wxCMD_LINE_VAL_STRING,
              wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_OPTION_MANDATORY },

        wxCMD_LINE_DESC_END 
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
    flagGettext = cmdline.Found("g");
    flagVerbose = cmdline.Found("v");
    flagCPP = cmdline.Found("c");
    flagPython = cmdline.Found("p");
    flagH = flagCPP && cmdline.Found("e");


    if (!cmdline.Found("o", &parOutput))
    {
        if (flagGettext)
            parOutput = wxEmptyString;
        else
        {
            if (flagCPP)
                parOutput = _T("resource.cpp");
            else if (flagPython)
                parOutput = _T("resource.py");
            else
                parOutput = _T("resource.xrs");
        }
    }
    if (!parOutput.empty())
    {
        wxFileName fn(parOutput);
        fn.Normalize();
        parOutput = fn.GetFullPath();
        parOutputPath = wxPathOnly(parOutput);
    }
    if (!parOutputPath) parOutputPath = _T(".");

    if (!cmdline.Found("n", &parFuncname))
        parFuncname = _T("InitXmlResource");

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
    name2.Replace(_T(":"), _T("_"));
    name2.Replace(_T("/"), _T("_"));
    name2.Replace(_T("\\"), _T("_"));
    name2.Replace(_T("*"), _T("_"));
    name2.Replace(_T("?"), _T("_"));

    wxString s = wxFileNameFromPath(parOutput) + _T("$") + name2;

    if (wxFileExists(s) && flist.Index(s) == wxNOT_FOUND)
    {
        for (int i = 0;; i++)
        {
            s.Printf(wxFileNameFromPath(parOutput) + _T("$%03i-") + name2, i);
            if (!wxFileExists(s) || flist.Index(s) != wxNOT_FOUND)
                break;
        }
    }
    return s;
}

wxArrayString XmlResApp::PrepareTempFiles()
{
    wxArrayString flist;

    for (size_t i = 0; i < parFiles.GetCount(); i++)
    {
        if (flagVerbose)
            wxPrintf(_T("processing ") + parFiles[i] +  _T("...\n"));

        wxXmlDocument doc;

        if (!doc.Load(parFiles[i]))
        {
            wxLogError(_T("Error parsing file ") + parFiles[i]);
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
                    if(node->GetName() == _T("object")
                     && node->GetAttribute(_T("class"),&classValue)
                     && node->GetAttribute(_T("name"),&nameValue)){

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
   if ( name == _T("bitmap") || name == _T("bitmap2") )
       return true;

   if ( name == _T("icon") )
       return true;

   // wxBitmapButton:
   wxXmlNode *parent = node->GetParent();
   if (parent != NULL &&
       parent->GetAttribute(_T("class"), _T("")) == _T("wxBitmapButton") &&
       (name == _T("focus") ||
        name == _T("disabled") ||
        name == _T("hover") ||
        name == _T("selected")))
       return true;

   // wxBitmap or wxIcon toplevel resources:
   if ( name == _T("object") )
   {
       wxString klass = node->GetAttribute(_T("class"), wxEmptyString);
       if (klass == _T("wxBitmap") ||
               klass == _T("wxIcon") ||
                klass == _T("data") )
           return true;
   }

   // URLs in wxHtmlWindow:
   if ( name == _T("url") &&
        parent != NULL &&
        parent->GetAttribute(_T("class"), _T("")) == _T("wxHtmlWindow") )
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
                wxPrintf(_T("adding     ") + fullname +  _T("...\n"));

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
    for (size_t i = 0; i < flist.GetCount(); i++)
        wxRemoveFile(parOutputPath + wxFILE_SEP_PATH + flist[i]);
}



void XmlResApp::MakePackageZIP(const wxArrayString& flist)
{
    wxString files;

    for (size_t i = 0; i < flist.GetCount(); i++)
        files += flist[i] + _T(" ");
    files.RemoveLast();

    if (flagVerbose)
        wxPrintf(_T("compressing ") + parOutput +  _T("...\n"));

    wxString cwd = wxGetCwd();
    wxSetWorkingDirectory(parOutputPath);
    int execres = wxExecute(_T("zip -9 -j ") +
                            wxString(flagVerbose ? _T("\"") : _T("-q \"")) +
                            parOutput + _T("\" ") + files, true);
    wxSetWorkingDirectory(cwd);
    if (execres == -1)
    {
        wxLogError(_T("Unable to execute zip program. Make sure it is in the path."));
        wxLogError(_T("You can download it at http://www.cdrom.com/pub/infozip/"));
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
    wxASSERT_MSG( static_cast<wxFileOffset>(lng) == offset,
                  wxT("Huge file not supported") );

    snum.Printf(_T("%i"), num);
    output.Printf(_T("static size_t xml_res_size_") + snum + _T(" = %i;\n"), lng);
    output += _T("static unsigned char xml_res_file_") + snum + _T("[] = {\n");
    // we cannot use string literals because MSVC is dumb wannabe compiler
    // with arbitrary limitation to 2048 strings :(

    unsigned char *buffer = new unsigned char[lng];
    file.Read(buffer, lng);

    for (size_t i = 0, linelng = 0; i < lng; i++)
    {
        tmp.Printf(_T("%i"), buffer[i]);
        if (i != 0) output << _T(',');
        if (linelng > 70)
        {
            linelng = 0;
            output << _T("\n");
        }
        output << tmp;
        linelng += tmp.Length()+1;
    }

    delete[] buffer;

    output += _T("};\n\n");

    return output;
}


void XmlResApp::MakePackageCPP(const wxArrayString& flist)
{
    wxFFile file(parOutput, wxT("wt"));
    size_t i;

    if (flagVerbose)
        wxPrintf(_T("creating C++ source file ") + parOutput +  _T("...\n"));

    file.Write(""
"//\n"
"// This file was automatically generated by wxrc, do not edit by hand.\n"
"//\n\n"
"#include <wx/wxprec.h>\n"
"\n"
"#ifdef __BORLANDC__\n"
"    #pragma hdrstop\n"
"#endif\n"
"\n"
""
"#include <wx/filesys.h>\n"
"#include <wx/fs_mem.h>\n"
"#include <wx/xrc/xmlres.h>\n"
"#include <wx/xrc/xh_all.h>\n"
"\n"
"#if wxCHECK_VERSION(2,8,5) && wxABI_VERSION >= 20805\n"
"    #define XRC_ADD_FILE(name, data, size, mime) \\\n"
"        wxMemoryFSHandler::AddFileWithMimeType(name, data, size, mime)\n"
"#else\n"
"    #define XRC_ADD_FILE(name, data, size, mime) \\\n"
"        wxMemoryFSHandler::AddFile(name, data, size)\n"
"#endif\n"
"\n");

    for (i = 0; i < flist.GetCount(); i++)
        file.Write(
              FileToCppArray(parOutputPath + wxFILE_SEP_PATH + flist[i], i));

    file.Write(""
"void " + parFuncname + "()\n"
"{\n"
"\n"
"    // Check for memory FS. If not present, load the handler:\n"
"    {\n"
"        wxMemoryFSHandler::AddFile(wxT(\"XRC_resource/dummy_file\"), wxT(\"dummy one\"));\n"
"        wxFileSystem fsys;\n"
"        wxFSFile *f = fsys.OpenFile(wxT(\"memory:XRC_resource/dummy_file\"));\n"
"        wxMemoryFSHandler::RemoveFile(wxT(\"XRC_resource/dummy_file\"));\n"
"        if (f) delete f;\n"
"        else wxFileSystem::AddHandler(new wxMemoryFSHandler);\n"
"    }\n"
"\n");

    for (i = 0; i < flist.GetCount(); i++)
    {
        wxString s;

        wxString mime;
        wxString ext = wxFileName(flist[i]).GetExt();
        if ( ext.Lower() == _T("xrc") )
            mime = _T("text/xml");
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

        s.Printf("    XRC_ADD_FILE(wxT(\"XRC_resource/" + flist[i] +
                 "\"), xml_res_file_%i, xml_res_size_%i, _T(\"%s\"));\n",
                 i, i, mime.c_str());
        file.Write(s);
    }

    for (i = 0; i < parFiles.GetCount(); i++)
    {
        file.Write("    wxXmlResource::Get()->Load(wxT(\"memory:XRC_resource/" +
                   GetInternalFileName(parFiles[i], flist) + "\"));\n");
    }

    file.Write("}\n");


}

void XmlResApp::GenCPPHeader()
{
    wxString fileSpec = ((parOutput.BeforeLast('.')).AfterLast('/')).AfterLast('\\');
    wxString heaFileName = fileSpec + _T(".h");

    wxFFile file(heaFileName, wxT("wt"));
    file.Write(
"//\n"
"// This file was automatically generated by wxrc, do not edit by hand.\n"
"//\n\n"
"#ifndef __"  + fileSpec + "_h__\n"
"#define __"  + fileSpec + "_h__\n"
);
    for(size_t i=0;i<aXRCWndClassData.GetCount();++i){
                aXRCWndClassData.Item(i).GenerateHeaderCode(file);
    }
    file.Write(
                "\nvoid \n"
                + parFuncname
                + "();\n#endif\n");
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
    wxASSERT_MSG( static_cast<wxFileOffset>(lng) == offset,
                  wxT("Huge file not supported") );

    snum.Printf(_T("%i"), num);
    output = "    xml_res_file_" + snum + " = '''\\\n";

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
            tmp.Printf(_T("\\x%02x"), c);
        else if (c == '\\')
            tmp = _T("\\\\");
        else
            tmp = (wxChar)c;
        if (linelng > 70)
        {
            linelng = 0;
            output << _T("\\\n");
        }
        output << tmp;
        linelng += tmp.Length();
    }

    delete[] buffer;

    output += _T("'''\n\n");

    return output;
}


void XmlResApp::MakePackagePython(const wxArrayString& flist)
{
    wxFFile file(parOutput, wxT("wt"));
    size_t i;

    if (flagVerbose)
        wxPrintf(_T("creating Python source file ") + parOutput +  _T("...\n"));

    file.Write(
       "#\n"
       "# This file was automatically generated by wxrc, do not edit by hand.\n"
       "#\n\n"
       "import wx\n"
       "import wx.xrc\n\n"
    );


    file.Write("def " + parFuncname + "():\n");

    for (i = 0; i < flist.GetCount(); i++)
        file.Write(
          FileToPythonArray(parOutputPath + wxFILE_SEP_PATH + flist[i], i));

    file.Write(
        "    # check if the memory filesystem handler has been loaded yet, and load it if not\n"
        "    wx.MemoryFSHandler.AddFile('XRC_resource/dummy_file', 'dummy value')\n"
        "    fsys = wx.FileSystem()\n"
        "    f = fsys.OpenFile('memory:XRC_resource/dummy_file')\n"
        "    wx.MemoryFSHandler.RemoveFile('XRC_resource/dummy_file')\n"
        "    if f is not None:\n"
        "        f.Destroy()\n"
        "    else:\n"
        "        wx.FileSystem.AddHandler(wx.MemoryFSHandler())\n"
        "\n"
        "    # load all the strings as memory files and load into XmlRes\n"
        );


    for (i = 0; i < flist.GetCount(); i++)
    {
        wxString s;
        s.Printf("    wx.MemoryFSHandler.AddFile('XRC_resource/" + flist[i] +
                 "', xml_res_file_%i)\n", i);
        file.Write(s);
    }
    for (i = 0; i < parFiles.GetCount(); i++)
    {
        file.Write("    wx.xrc.XmlResource.Get().Load('memory:XRC_resource/" +
                   GetInternalFileName(parFiles[i], flist) + "')\n");
    }

    file.Write("\n");
}



void XmlResApp::OutputGettext()
{
    ExtractedStrings str = FindStrings();

    wxFFile fout;
    if (parOutput.empty())
        fout.Attach(stdout);
    else
        fout.Open(parOutput, wxT("wt"));

    for (ExtractedStrings::const_iterator i = str.begin(); i != str.end(); ++i)
    {
        wxString s;

        s.Printf("#line %d \"%s\"\n", i->lineNo, i->filename);
        fout.Write(s);
        fout.Write("_(\"" + i->str + "\");\n");
    }

    if (!parOutput) fout.Detach();
}



ExtractedStrings XmlResApp::FindStrings()
{
    ExtractedStrings arr, a2;

    for (size_t i = 0; i < parFiles.GetCount(); i++)
    {
        if (flagVerbose)
            wxPrintf(_T("processing ") + parFiles[i] +  _T("...\n"));

        wxXmlDocument doc;
        if (!doc.Load(parFiles[i]))
        {
            wxLogError(_T("Error parsing file ") + parFiles[i]);
            retCode = 1;
            continue;
        }
        a2 = FindStrings(parFiles[i], doc.GetRoot());

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


ExtractedStrings
XmlResApp::FindStrings(const wxString& filename, wxXmlNode *node)
{
    ExtractedStrings arr;

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
                node/*not n!*/->GetName() == _T("label") ||
                (node/*not n!*/->GetName() == _T("value") &&
                               !n->GetContent().IsNumber()) ||
                node/*not n!*/->GetName() == _T("help") ||
                node/*not n!*/->GetName() == _T("longhelp") ||
                node/*not n!*/->GetName() == _T("tooltip") ||
                node/*not n!*/->GetName() == _T("htmlcode") ||
                node/*not n!*/->GetName() == _T("title") ||
                node/*not n!*/->GetName() == _T("item")
            ))
            // ...and known to contain translatable string
        {
            if (!flagGettext ||
                node->GetAttribute(_T("translate"), _T("1")) != _T("0"))
            {
                arr.push_back
                    (
                        ExtractedString
                        (
                            ConvertText(n->GetContent()),
                            filename,
                            n->GetLineNumber()
                        )
                    );
            }
        }

        // subnodes:
        if (n->GetType() == wxXML_ELEMENT_NODE)
        {
            ExtractedStrings a2 = FindStrings(filename, n);
            WX_APPEND_ARRAY(arr, a2);
        }

        n = n->GetNext();
    }
    return arr;
}
