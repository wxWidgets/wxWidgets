/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:     Utility functions and classes
// Author:      Julian Smart
// Modified by:
// Created:     2002-09-04
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma implementation
#endif

#include "wx/wx.h"
#include "wx/image.h"
#include "wx/notebook.h"
#include "wx/splitter.h"
#include "wx/wfstream.h"
#include "wx/datstrm.h"
#include "wx/file.h"
#include "wx/listctrl.h"
#include "wx/tokenzr.h"
#include "wx/process.h"
#include "wx/mimetype.h"
#include "wx/variant.h"
#include "wx/cshelp.h"
#include "wx/cmdline.h"

#include <math.h>

#ifdef __WXMSW__
#include <windows.h>
#include "wx/msw/winundef.h"
#endif

#include "utils.h"

// Returns the image type, or -1, determined from the extension.
int apDetermineImageType(const wxString& filename)
{
    wxString path, name, ext;

    wxSplitPath(filename, & path, & name, & ext);

    ext.MakeLower();
    if (ext == "jpg" || ext == "jpeg")
        return wxBITMAP_TYPE_JPEG;
    else if (ext == "gif")
        return wxBITMAP_TYPE_GIF;
    else if (ext == "bmp")
        return wxBITMAP_TYPE_BMP;
    else if (ext == "png")
        return wxBITMAP_TYPE_PNG;
    else if (ext == "pcx")
        return wxBITMAP_TYPE_PCX;
    else if (ext == "tif" || ext == "tiff")
        return wxBITMAP_TYPE_TIF;
    else
        return -1;
}

// Convert a colour to a 6-digit hex string
wxString apColourToHexString(const wxColour& col)
{
    wxString hex;

    hex += wxDecToHex(col.Red());
    hex += wxDecToHex(col.Green());
    hex += wxDecToHex(col.Blue());

    return hex;
}

// Convert 6-digit hex string to a colour
wxColour apHexStringToColour(const wxString& hex)
{
    unsigned int r = 0;
    unsigned int g = 0;
    unsigned int b = 0;
    r = wxHexToDec(hex.Mid(0, 2));
    g = wxHexToDec(hex.Mid(2, 2));
    b = wxHexToDec(hex.Mid(4, 2));

    return wxColour(r, g, b);
}

// Convert a wxFont to a string
wxString apFontToString(const wxFont& font)
{
    wxString str;
    str.Printf(wxT("%d,%d,%d,%d,%d,%s"), (int) font.GetPointSize(),
        (int) font.GetFamily(), (int) font.GetStyle(), (int) font.GetWeight(),
        (int) font.GetUnderlined(), font.GetFaceName().c_str());

    return str;
}

// Convert a string to a wxFont
wxFont apStringToFont(const wxString& str)
{
    int pointSize = 12;
    int family = wxSWISS;
    int style = wxNORMAL;
    int weight = wxNORMAL;
    int underlined = 0;
    wxString facename(wxT(""));

    wxStringTokenizer tkz(str, wxT(","));
    int i = 0;
    while (tkz.HasMoreTokens())
    {
        wxString token = tkz.GetNextToken();

        if (i == 0)
        {
            StringToInt(token, & pointSize);
#if defined(__WXGTK__) || defined(__WXMAC__)
            if (pointSize < 8)
                pointSize = 8;
            if (pointSize == 9)
                pointSize = 10;
#endif            
        }
        else if (i == 1)
            StringToInt(token, & family);
        else if (i == 2)
            StringToInt(token, & style);
        else if (i == 3)
            StringToInt(token, & weight);
        else if (i == 4)
            StringToInt(token, & underlined);
        else if (i == 5)
        {
            facename = token;
#if defined(__WXGTK__)
            if (facename == wxT("Arial"))
                facename = wxT("helvetica");
#endif
        }
        i ++;

    }
    return wxFont(pointSize, family, style, weight, (underlined != 0), facename);
}


// Get the index of the given named wxNotebook page
int apFindNotebookPage(wxNotebook* notebook, const wxString& name)
{
    int i;
    for (i = 0; i < notebook->GetPageCount(); i++)
        if (name == notebook->GetPageText(i))
            return i;
    return -1;
}

/*
 * View an HTML file
 */

void apViewHTMLFile(const wxString& url)
{
#ifdef __WXMSW__
    HKEY hKey;
    TCHAR szCmdName[1024];
    DWORD dwType, dw = sizeof(szCmdName);
    LONG lRes;
    lRes = RegOpenKey(HKEY_CLASSES_ROOT, "htmlfile\\shell\\open\\command", &hKey);
    if(lRes == ERROR_SUCCESS && RegQueryValueEx(hKey,(LPTSTR)NULL, NULL,
        &dwType, (LPBYTE)szCmdName, &dw) == ERROR_SUCCESS)
    {
        strcat(szCmdName, (const char*) url);
        PROCESS_INFORMATION  piProcInfo;
        STARTUPINFO          siStartInfo;
        memset(&siStartInfo, 0, sizeof(STARTUPINFO));
        siStartInfo.cb = sizeof(STARTUPINFO);
        CreateProcess(NULL, szCmdName, NULL, NULL, FALSE, 0, NULL,
            NULL, &siStartInfo, &piProcInfo );
    }
    if(lRes == ERROR_SUCCESS)
        RegCloseKey(hKey);
#else
    wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(wxT("html"));
    if ( !ft )
    {
        wxLogError(_T("Impossible to determine the file type for extension html. Please edit your MIME types."));
        return ;
    }

    wxString cmd;
    bool ok = ft->GetOpenCommand(&cmd,
                                 wxFileType::MessageParameters(url, _T("")));
    delete ft;

    if (!ok)
    {
        // TODO: some kind of configuration dialog here.
        wxMessageBox(_("Could not determine the command for running the browser."),
		   wxT("Browsing problem"), wxOK|wxICON_EXCLAMATION);
        return ;
    }

    ok = (wxExecute(cmd, FALSE) != 0);
#endif
}

wxString wxGetTempDir()
{
    wxString dir;
#if defined(__WXMAC__) && !defined(__DARWIN__)
    dir = wxMacFindFolder(  (short) kOnSystemDisk, kTemporaryFolderType, kCreateFolder ) ;
#else // !Mac
    dir = wxGetenv(_T("TMP"));
    if ( dir.empty() )
    {
        dir = wxGetenv(_T("TEMP"));
    }
    
    if ( dir.empty() )
    {
        // default
#ifdef __DOS__
        dir = _T(".");
#else
        dir = _T("/tmp");
#endif
    }
#endif // Mac/!Mac
    return dir;
}

// Invoke app for file type
// Eventually we should allow the user to select an app.
bool apInvokeAppForFile(const wxString& filename)
{
    wxString path, file, ext;
    wxSplitPath(filename, & path, & file, & ext);

    wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);
    if ( !ft )
    {
        wxString msg;
        msg.Printf(wxT("Sorry, could not determine what application to invoke for extension %s\nYou may need to edit your MIME types."),
            ext.c_str());
        wxMessageBox(msg, wxT("Application Invocation"), wxICON_EXCLAMATION|wxOK);
        return FALSE;
    }

    wxString cmd;
    bool ok = ft->GetOpenCommand(&cmd,
                                 wxFileType::MessageParameters(filename, _T("")));
    delete ft;

    ok = (wxExecute(cmd, FALSE) != 0);

    return ok;
}

// Find the absolute path where this application has been run from.
// argv0 is wxTheApp->argv[0]
// cwd is the current working directory (at startup)
// appVariableName is the name of a variable containing the directory for this app, e.g.
// MYAPPDIR. This is checked first.

wxString apFindAppPath(const wxString& argv0, const wxString& cwd, const wxString& appVariableName)
{
    wxString str;

    // Try appVariableName
    if (!appVariableName.IsEmpty())
    {
        str = wxGetenv(appVariableName);
        if (!str.IsEmpty())
            return str;
    }

    if (wxIsAbsolutePath(argv0))
        return wxPathOnly(argv0);
    else
    {
        // Is it a relative path?
        wxString currentDir(cwd);
        if (currentDir.Last() != wxFILE_SEP_PATH)
            currentDir += wxFILE_SEP_PATH;

        str = currentDir + argv0;
        if (wxFileExists(str))
            return wxPathOnly(str);
    }

    // OK, it's neither an absolute path nor a relative path.
    // Search PATH.

    wxPathList pathList;
    pathList.AddEnvList(wxT("PATH"));
    str = pathList.FindAbsoluteValidPath(argv0);
    if (!str.IsEmpty())
        return wxPathOnly(str);

    // Failed
    return wxEmptyString;
}

// Adds a context-sensitive help button, for non-Windows platforms
void apAddContextHelpButton(wxWindow* parent, wxSizer* sizer, int sizerFlags, int sizerBorder)
{
#if defined(__WXGTK__) || defined(__WXMAC__)
#ifdef __WXMAC__
    wxSize buttonSize(20, 20);
#else
    wxSize buttonSize(-1, -1);
#endif
    wxButton *contextButton = new wxContextHelpButton( parent, wxID_CONTEXT_HELP,
        wxDefaultPosition, buttonSize);
    sizer->Add( contextButton, 0, sizerFlags, sizerBorder );

    // Add a bit of space on the right, to allow for the dialog resizing
    // handle
#ifdef __WXMAC__
    sizer->Add(0, 0, 0, wxRIGHT, 10);
#endif

    contextButton->SetHelpText(_("Invokes context-sensitive help for the clicked-on window."));
    if (wxGetApp().UsingTooltips())
    {
        contextButton->SetToolTip(_("Invokes context-sensitive help for the clicked-on window."));
    }
#endif
}

// Get selected wxNotebook page
wxWindow* apNotebookGetSelectedPage(wxNotebook* notebook)
{
    int sel = notebook->GetSelection();
    if (sel > -1)
    {
        return notebook->GetPage(sel);
    }
    return NULL;
}

/*
* wxIconInfo
*/

wxIconInfo::wxIconInfo(const wxString& name)
{
    m_maxStates = 0;
    m_name = name;
    int i;
    for (i = 0; i < wxMAX_ICON_STATES; i++)
        m_states[i] = 0;
}

int wxIconInfo::GetIconId(int state, bool enabled) const
{
    wxASSERT ( state < (wxMAX_ICON_STATES * 2) );
    wxASSERT ( state < m_maxStates );
    
    return m_states[state * 2 + (enabled ? 0 : 1)];
}

void wxIconInfo::SetIconId(int state, bool enabled, int iconId)
{
    wxASSERT ( state < (wxMAX_ICON_STATES * 2) );
    if (state+1 > m_maxStates)
        m_maxStates = state+1;
    
    m_states[state * 2 + (enabled ? 0 : 1)] = iconId;
}

/*
* wxIconTable
* Contains a list of wxIconInfos
*/

wxIconTable::wxIconTable(wxImageList* imageList)
{
    m_imageList = imageList;
    DeleteContents(TRUE);
}

void wxIconTable::AppendInfo(wxIconInfo* info)
{
    Append(info);
}

// Easy way of initialising both the image list and the
// table. It will generate image ids itself while appending the icon.
bool wxIconTable::AddInfo(const wxString& name, const wxIcon& icon, int state, bool enabled)
{
    wxASSERT (m_imageList != NULL);
    
    wxIconInfo* info = FindInfo(name);
    if (!info)
    {
        info = new wxIconInfo(name);
        Append(info);
    }
    info->SetIconId(state, enabled, m_imageList->Add(icon));
    return TRUE;
}

wxIconInfo* wxIconTable::FindInfo(const wxString& name) const
{
    wxNode* node = First();
    while (node)
    {
        wxIconInfo* info = (wxIconInfo*) node->Data();
        if (info->GetName() == name)
            return info;
        node = node->Next();
    }
    return NULL;
}

int wxIconTable::GetIconId(const wxString& name, int state, bool enabled) const
{
    wxIconInfo* info = FindInfo(name);
    if (!info)
        return -1;
    return info->GetIconId(state, enabled);
}

bool wxIconTable::SetIconId(const wxString& name, int state, bool enabled, int iconId)
{
    wxIconInfo* info = FindInfo(name);
    if (!info)
        return FALSE;
    info->SetIconId(state, enabled, iconId);
    return TRUE;
}

// Output stream operators

wxOutputStream& operator <<(wxOutputStream& stream, const wxString& s)
{
    stream.Write(s, s.Length());
    return stream;
}

wxOutputStream& operator <<(wxOutputStream& stream, long l)
{
    wxString str;
    str.Printf(_T("%ld"), l);
    return stream << str;
}

wxOutputStream& operator <<(wxOutputStream& stream, const char c)
{
    wxString str;
    str.Printf(_T("%c"), c);
    return stream << str;
}

// Convert characters to HTML equivalents
wxString ctEscapeHTMLCharacters(const wxString& str)
{
    wxString s;
    size_t len = str.Length();
    size_t i;
    for (i = 0; i < len; i++)
    {
        wxChar c = str.GetChar(i);
        if (c == _T('<'))
            s += _T("&lt;");
        else if (c == _T('>'))
            s += _T("&gt;");
        else if (c == _T('&'))
            s += _T("&amp;");
        else
            s += c;
    }
    return s;
}