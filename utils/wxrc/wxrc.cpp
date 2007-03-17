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
#endif

#include "wx/cmdline.h"
#include "wx/xml/xml.h"
#include "wx/ffile.h"
#include "wx/filename.h"
#include "wx/wfstream.h"
#include "wx/utils.h"
#include "wx/hashset.h"

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
                && node->GetPropVal(_T("class"),&classValue)
                && node->GetPropVal(_T("name"),&nameValue))
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
        for(i=0;i<m_wdata.Count();++i)
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
        for(i=0;i<m_wdata.Count();++i)
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
        { wxCMD_LINE_SWITCH, _T("h"), _T("help"),  _T("show help message"),
              wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, _T("v"), _T("verbose"), _T("be verbose"), (wxCmdLineParamType)0, 0 },
        { wxCMD_LINE_SWITCH, _T("e"), _T("extra-cpp-code"),  _T("output C++ header file with XRC derived classes"), (wxCmdLineParamType)0, 0 },
        { wxCMD_LINE_SWITCH, _T("c"), _T("cpp-code"),  _T("output C++ source rather than .rsc file"), (wxCmdLineParamType)0, 0 },
        { wxCMD_LINE_SWITCH, _T("p"), _T("python-code"),  _T("output wxPython source rather than .rsc file"), (wxCmdLineParamType)0, 0 },
        { wxCMD_LINE_SWITCH, _T("g"), _T("gettext"),  _T("output list of translatable strings (to stdout or file if -o used)"), (wxCmdLineParamType)0, 0 },
        { wxCMD_LINE_OPTION, _T("n"), _T("function"),  _T("C++/Python function name (with -c or -p) [InitXmlResource]"), (wxCmdLineParamType)0, 0 },
        { wxCMD_LINE_OPTION, _T("o"), _T("output"),  _T("output file [resource.xrs/cpp]"), (wxCmdLineParamType)0, 0 },
#if 0 // not yet implemented
        { wxCMD_LINE_OPTION, _T("l"), _T("list-of-handlers"),  _T("output list of necessary handlers to this file"), (wxCmdLineParamType)0, 0 },
#endif
        { wxCMD_LINE_PARAM,  NULL, NULL, _T("input file(s)"),
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
    flagGettext = cmdline.Found(_T("g"));
    flagVerbose = cmdline.Found(_T("v"));
    flagCPP = cmdline.Found(_T("c"));
    flagPython = cmdline.Found(_T("p"));
    flagH = flagCPP && cmdline.Found(_T("e"));


    if (!cmdline.Found(_T("o"), &parOutput))
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

    if (!cmdline.Found(_T("n"), &parFuncname))
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

    for (size_t i = 0; i < parFiles.Count(); i++)
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
                     && node->GetPropVal(_T("class"),&classValue)
                     && node->GetPropVal(_T("name"),&nameValue)){

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

   // URLs in wxHtmlWindow:
   if ( name == _T("url") )
       return true;

   // wxBitmapButton:
   wxXmlNode *parent = node->GetParent();
   if (parent != NULL &&
       parent->GetPropVal(_T("class"), _T("")) == _T("wxBitmapButton") &&
       (name == _T("focus") ||
        name == _T("disabled") ||
        name == _T("selected")))
       return true;

   // wxBitmap or wxIcon toplevel resources:
   if ( name == _T("object") )
   {
       wxString klass = node->GetPropVal(_T("class"), wxEmptyString);
       if (klass == _T("wxBitmap") || klass == _T("wxIcon"))
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
    for (size_t i = 0; i < flist.Count(); i++)
        wxRemoveFile(parOutputPath + wxFILE_SEP_PATH + flist[i]);
}



void XmlResApp::MakePackageZIP(const wxArrayString& flist)
{
    wxString files;

    for (size_t i = 0; i < flist.Count(); i++)
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
    wxASSERT_MSG( lng == offset, wxT("Huge file not supported") );

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

    file.Write(_T("")
_T("//\n")
_T("// This file was automatically generated by wxrc, do not edit by hand.\n")
_T("//\n\n")
_T("#include <wx/wxprec.h>\n")
_T("\n")
_T("#ifdef __BORLANDC__\n")
_T("    #pragma hdrstop\n")
_T("#endif\n")
_T("\n")
_T("")
_T("#include <wx/filesys.h>\n")
_T("#include <wx/fs_mem.h>\n")
_T("#include <wx/xrc/xmlres.h>\n")
_T("#include <wx/xrc/xh_all.h>\n")
_T("\n"));

    for (i = 0; i < flist.Count(); i++)
        file.Write(
              FileToCppArray(parOutputPath + wxFILE_SEP_PATH + flist[i], i));

    file.Write(_T("")
_T("void ") + parFuncname + wxT("()\n")
_T("{\n")
_T("\n")
_T("    // Check for memory FS. If not present, load the handler:\n")
_T("    {\n")
_T("        wxMemoryFSHandler::AddFile(wxT(\"XRC_resource/dummy_file\"), wxT(\"dummy one\"));\n")
_T("        wxFileSystem fsys;\n")
_T("        wxFSFile *f = fsys.OpenFile(wxT(\"memory:XRC_resource/dummy_file\"));\n")
_T("        wxMemoryFSHandler::RemoveFile(wxT(\"XRC_resource/dummy_file\"));\n")
_T("        if (f) delete f;\n")
_T("        else wxFileSystem::AddHandler(new wxMemoryFSHandler);\n")
_T("    }\n")
_T("\n"));

    for (i = 0; i < flist.Count(); i++)
    {
        wxString s;
        s.Printf(_T("    wxMemoryFSHandler::AddFile(wxT(\"XRC_resource/") + flist[i] +
                 _T("\"), xml_res_file_%i, xml_res_size_%i);\n"), i, i);
        file.Write(s);
    }

    for (i = 0; i < parFiles.Count(); i++)
    {
        file.Write(_T("    wxXmlResource::Get()->Load(wxT(\"memory:XRC_resource/") +
                   GetInternalFileName(parFiles[i], flist) + _T("\"));\n"));
    }

    file.Write(_T("}\n"));


}

void XmlResApp::GenCPPHeader()
{
    wxString fileSpec = ((parOutput.BeforeLast('.')).AfterLast('/')).AfterLast('\\');
    wxString heaFileName = fileSpec + _T(".h");

    wxFFile file(heaFileName, wxT("wt"));
    file.Write(
_T("//\n")
_T("// This file was automatically generated by wxrc, do not edit by hand.\n")
_T("//\n\n")
_T("#ifndef __")  + fileSpec + _T("_h__\n")
_T("#define __")  + fileSpec + _T("_h__\n")
);
    for(size_t i=0;i<aXRCWndClassData.Count();++i){
                aXRCWndClassData.Item(i).GenerateHeaderCode(file);
    }
    file.Write(
                _T("\nvoid \n")
                + parFuncname
                + _T("();\n#endif\n"));
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
    wxASSERT_MSG( offset == lng, wxT("Huge file not supported") );

    snum.Printf(_T("%i"), num);
    output = _T("    xml_res_file_") + snum + _T(" = '''\\\n");

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
       _T("#\n")
       _T("# This file was automatically generated by wxrc, do not edit by hand.\n")
       _T("#\n\n")
       _T("import wx\n")
       _T("import wx.xrc\n\n")
    );


    file.Write(_T("def ") + parFuncname + _T("():\n"));

    for (i = 0; i < flist.Count(); i++)
        file.Write(
          FileToPythonArray(parOutputPath + wxFILE_SEP_PATH + flist[i], i));

    file.Write(
        _T("    # check if the memory filesystem handler has been loaded yet, and load it if not\n")
        _T("    wx.MemoryFSHandler.AddFile('XRC_resource/dummy_file', 'dummy value')\n")
        _T("    fsys = wx.FileSystem()\n")
        _T("    f = fsys.OpenFile('memory:XRC_resource/dummy_file')\n")
        _T("    wx.MemoryFSHandler.RemoveFile('XRC_resource/dummy_file')\n")
        _T("    if f is not None:\n")
        _T("        f.Destroy()\n")
        _T("    else:\n")
        _T("        wx.FileSystem.AddHandler(wx.MemoryFSHandler())\n")
        _T("\n")
        _T("    # load all the strings as memory files and load into XmlRes\n")
        );


    for (i = 0; i < flist.Count(); i++)
    {
        wxString s;
        s.Printf(_T("    wx.MemoryFSHandler.AddFile('XRC_resource/") + flist[i] +
                 _T("', xml_res_file_%i)\n"), i);
        file.Write(s);
    }
    for (i = 0; i < parFiles.Count(); i++)
    {
        file.Write(_T("    wx.xrc.XmlResource.Get().Load('memory:XRC_resource/") +
                   GetInternalFileName(parFiles[i], flist) + _T("')\n"));
    }

    file.Write(_T("\n"));
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
        fout.Write(_T("_(\"") + str[i] + _T("\");\n"));

    if (!parOutput) fout.Detach();
}



wxArrayString XmlResApp::FindStrings()
{
    wxArrayString arr, a2;

    for (size_t i = 0; i < parFiles.Count(); i++)
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
                node->GetPropVal(_T("translate"), _T("1")) != _T("0"))
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
