/////////////////////////////////////////////////////////////////////////////
// Name:        fs_zip.cpp
// Purpose:     ZIP file system
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// CVS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////



#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "fs_zip.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_FILESYSTEM && wxUSE_FS_ZIP && wxUSE_ZIPSTREAM && wxUSE_ZLIB

#ifndef WXPRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include "wx/filesys.h"
#include "wx/wfstream.h"
#include "wx/zipstrm.h"
#include "wx/fs_zip.h"


//----------------------------------------------------------------------------
// wxZipFSHandler
//----------------------------------------------------------------------------



wxZipFSHandler::wxZipFSHandler() : wxFileSystemHandler()
{
    m_Archive = NULL;
    m_ZipFile = m_Pattern = m_BaseDir = wxEmptyString;
    m_AllowDirs = m_AllowFiles = true;
    m_DirsFound = NULL;
}



wxZipFSHandler::~wxZipFSHandler()
{
    if (m_Archive)
        CloseArchive(m_Archive);
    if (m_DirsFound)
        delete m_DirsFound;
}



void wxZipFSHandler::CloseArchive(wxZipInputStream *archive)
{
    wxInputStream *stream = archive->GetFilterInputStream();
    delete archive;
    delete stream;
}



bool wxZipFSHandler::CanOpen(const wxString& location)
{
    wxString p = GetProtocol(location);
    return (p == wxT("zip")) &&
           (GetProtocol(GetLeftLocation(location)) == wxT("file"));
}




wxFSFile* wxZipFSHandler::OpenFile(wxFileSystem& WXUNUSED(fs), const wxString& location)
{
    wxString right = GetRightLocation(location);
    wxString left = GetLeftLocation(location);
    wxInputStream *s;

    if (GetProtocol(left) != wxT("file"))
    {
        wxLogError(_("ZIP handler currently supports only local files!"));
        return NULL;
    }

    if (right.Contains(wxT("./")))
    {
        if (right.GetChar(0) != wxT('/')) right = wxT('/') + right;
        wxFileName rightPart(right, wxPATH_UNIX);
        rightPart.Normalize(wxPATH_NORM_DOTS, wxT("/"), wxPATH_UNIX);
        right = rightPart.GetFullPath(wxPATH_UNIX);
    }

    if (right.GetChar(0) == wxT('/')) right = right.Mid(1);

    wxFileName leftFilename = wxFileSystem::URLToFileName(left);

    s = new wxZipInputStream(leftFilename.GetFullPath(), right);
    if (s && s->IsOk() )
    {
        return new wxFSFile(s,
                            left + wxT("#zip:") + right,
                            GetMimeTypeFromExt(location),
                            GetAnchor(location)
#if wxUSE_DATETIME
                            , wxDateTime(wxFileModificationTime(left))
#endif // wxUSE_DATETIME
                            );
    }

    delete s;
    return NULL;
}



wxString wxZipFSHandler::FindFirst(const wxString& spec, int flags)
{
    wxString right = GetRightLocation(spec);
    wxString left = GetLeftLocation(spec);

    if (right.Last() == wxT('/')) right.RemoveLast();

    if (m_Archive)
    {
        CloseArchive(m_Archive);
        m_Archive = NULL;
    }

    if (GetProtocol(left) != wxT("file"))
    {
        wxLogError(_("ZIP handler currently supports only local files!"));
        return wxEmptyString;
    }

    switch (flags)
    {
        case wxFILE:
            m_AllowDirs = false, m_AllowFiles = true; break;
        case wxDIR:
            m_AllowDirs = true, m_AllowFiles = false; break;
        default:
            m_AllowDirs = m_AllowFiles = true; break;
    }

    m_ZipFile = left;
    wxString nativename = wxFileSystem::URLToFileName(m_ZipFile).GetFullPath();

    wxFFileInputStream *fs = new wxFFileInputStream(nativename);
    if (fs->Ok())
        m_Archive = new wxZipInputStream(*fs);
    else
        delete fs;

    m_Pattern = right.AfterLast(wxT('/'));
    m_BaseDir = right.BeforeLast(wxT('/'));

    if (m_Archive)
    {
        if (m_AllowDirs)
        {
            delete m_DirsFound;
            m_DirsFound = new wxZipFilenameHashMap();
        }
        return DoFind();
    }
    return wxEmptyString;
}



wxString wxZipFSHandler::FindNext()
{
    if (!m_Archive) return wxEmptyString;
    return DoFind();
}



wxString wxZipFSHandler::DoFind()
{
    wxString namestr, dir, filename;
    wxString match = wxEmptyString;

    while (match == wxEmptyString)
    {
        wxZipEntry *entry = m_Archive->GetNextEntry();
        if (!entry)
        {
            CloseArchive(m_Archive);
            m_Archive = NULL;
            break;
        }
        namestr = entry->GetName(wxPATH_UNIX);
        delete entry;

        if (m_AllowDirs)
        {
            dir = namestr.BeforeLast(wxT('/'));
            while (!dir.IsEmpty())
            {
                if( m_DirsFound->find(dir) == m_DirsFound->end() )
                {
                    (*m_DirsFound)[dir] = 1;
                    filename = dir.AfterLast(wxT('/'));
                    dir = dir.BeforeLast(wxT('/'));
                    if (!filename.IsEmpty() && m_BaseDir == dir &&
                                wxMatchWild(m_Pattern, filename, false))
                        match = m_ZipFile + wxT("#zip:") + dir + wxT("/") + filename;
                }
                else
                    break; // already tranversed
            }
        }

        filename = namestr.AfterLast(wxT('/'));
        dir = namestr.BeforeLast(wxT('/'));
        if (m_AllowFiles && !filename.IsEmpty() && m_BaseDir == dir &&
                            wxMatchWild(m_Pattern, filename, false))
            match = m_ZipFile + wxT("#zip:") + namestr;
    }

    return match;
}



#endif
      //wxUSE_FILESYSTEM && wxUSE_FS_ZIP && wxUSE_ZIPSTREAM
