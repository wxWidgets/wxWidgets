/////////////////////////////////////////////////////////////////////////////
// Name:        fs_zip.cpp
// Purpose:     ZIP file system
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation
#endif

#include "wx/wxprec.h"

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#if wxUSE_FS_ZIP

#ifndef WXPRECOMP
#include "wx/wx.h"
#endif

#include "wx/filesys.h"
#include "wx/zipstrm.h"
#include "wx/fs_zip.h"

/* Not the right solution (paths in makefiles) but... */
#ifdef __BORLANDC__
#include "../common/unzip.h"
#else
#include "unzip.h"
#endif


//--------------------------------------------------------------------------------
// wxZipFSHandler
//--------------------------------------------------------------------------------



wxZipFSHandler::wxZipFSHandler() : wxFileSystemHandler()
{
    m_Archive = NULL;
    m_ZipFile = m_Pattern = m_BaseDir = wxEmptyString;
    m_AllowDirs = m_AllowFiles = TRUE;
}



wxZipFSHandler::~wxZipFSHandler()
{
    if (m_Archive)         
        unzClose((unzFile)m_Archive);
}



bool wxZipFSHandler::CanOpen(const wxString& location)
{
    wxString p = GetProtocol(location);
    return (p == wxT("zip") );
}




wxFSFile* wxZipFSHandler::OpenFile(wxFileSystem& WXUNUSED(fs), const wxString& location)
{
    wxString right = GetRightLocation(location);
    wxString left = GetLeftLocation(location);
    wxInputStream *s;

    if (GetProtocol(left) != wxT("file")) {
        return NULL;
    }

    s = new wxZipInputStream(left, right);
    if (s && (s -> LastError() == wxStream_NOERROR)) {
        return new wxFSFile(s,
                            left + wxT("#zip:") + right,
                            GetMimeTypeFromExt(location),
                            GetAnchor(location),
                            wxDateTime(wxFileModificationTime(left)));
    }
    else return NULL;
}



wxString wxZipFSHandler::FindFirst(const wxString& spec, int flags)
{
    wxString right = GetRightLocation(spec);
    wxString left = GetLeftLocation(spec);
    
    if (right.Last() == wxT('/')) right.RemoveLast();

    if (m_Archive) {
        unzClose((unzFile)m_Archive);
        m_Archive = NULL;
    }

    if (GetProtocol(left) != wxT("file")) {
        return wxEmptyString;
    }

    switch (flags) {
        case wxFILE  : m_AllowDirs = FALSE, m_AllowFiles = TRUE; break;
        case wxDIR   : m_AllowDirs = TRUE, m_AllowFiles = FALSE; break;
        default : m_AllowDirs = m_AllowFiles = TRUE; break;
    }

    m_ZipFile = left;
    m_Archive = (void*) unzOpen(m_ZipFile.fn_str());
    m_Pattern = right.AfterLast(wxT('/'));
    m_BaseDir = right.BeforeLast(wxT('/'));

    if (m_Archive) {
        if (unzGoToFirstFile((unzFile)m_Archive) != UNZ_OK) {
            unzClose((unzFile)m_Archive);
            m_Archive = NULL;        
        }
    }
    return DoFind();
}



wxString wxZipFSHandler::FindNext()
{
    if (!m_Archive) return wxEmptyString;
    return DoFind();
}



wxString wxZipFSHandler::DoFind()
{
    static char namebuf[1024]; // char, not wxChar!
    char *c;
    wxString fn, dir, name;
    wxString match = wxEmptyString;
    bool wasdir;

    while (match == wxEmptyString)
    {
        unzGetCurrentFileInfo((unzFile)m_Archive, NULL, namebuf, 1024, NULL, 0, NULL, 0);
        for (c = namebuf; *c; c++) if (*c == wxT('\\')) *c = wxT('/');
        fn = namebuf;
        if (fn.Last() == wxT('/')) {
            fn.RemoveLast();
            wasdir = TRUE;
        }
        else wasdir = FALSE;

        name = fn.AfterLast(wxT('/'));
        dir = fn.BeforeLast(wxT('/'));

        if (dir == m_BaseDir) {
            if (m_AllowFiles && !wasdir && wxMatchWild(m_Pattern, name, FALSE))
                match = m_ZipFile + wxT("#zip:") + fn;
            if (m_AllowDirs && wasdir && wxMatchWild(m_Pattern, name, FALSE))
                match = m_ZipFile + wxT("#zip:") + fn;
        }
        
        if (unzGoToNextFile((unzFile)m_Archive) != UNZ_OK) {
            unzClose((unzFile)m_Archive);
            m_Archive = NULL;
            break;
        }
    }
    
    return match;
}



#endif // wxUSE_FS_ZIP
