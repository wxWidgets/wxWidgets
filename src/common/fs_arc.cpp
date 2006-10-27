/////////////////////////////////////////////////////////////////////////////
// Name:        fs_zip.cpp
// Purpose:     ZIP file system
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// CVS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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


//---------------------------------------------------------------------------
// wxZipFSInputStream
//---------------------------------------------------------------------------
// Helper class for wxZipFSHandler

class wxZipFSInputStream : public wxZipInputStream
{
    public:
       wxZipFSInputStream(wxFSFile *file)
               : wxZipInputStream(*file->GetStream())
       {
            m_file = file;
#if WXWIN_COMPATIBILITY_2_6
            m_allowSeeking = true;
#endif
       }

       virtual ~wxZipFSInputStream() { delete m_file; }

    private:
       wxFSFile *m_file;
};

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
    Cleanup();
}


void wxZipFSHandler::Cleanup()
{
    wxDELETE(m_Archive);
    wxDELETE(m_DirsFound);
}
 


bool wxZipFSHandler::CanOpen(const wxString& location)
{
    wxString p = GetProtocol(location);
    return (p == wxT("zip"));
}


wxFSFile* wxZipFSHandler::OpenFile(wxFileSystem& WXUNUSED(fs), const wxString& location)
{
    wxString right = GetRightLocation(location);
    wxString left = GetLeftLocation(location);
    wxZipInputStream *s;

    if (right.Contains(wxT("./")))
    {
        if (right.GetChar(0) != wxT('/')) right = wxT('/') + right;
        wxFileName rightPart(right, wxPATH_UNIX);
        rightPart.Normalize(wxPATH_NORM_DOTS, wxT("/"), wxPATH_UNIX);
        right = rightPart.GetFullPath(wxPATH_UNIX);
    }

    if (right.GetChar(0) == wxT('/')) right = right.Mid(1);

    // a new wxFileSystem object is needed here to avoid infinite recursion
    wxFSFile *leftFile = wxFileSystem().OpenFile(left);
    if (!leftFile)
       return NULL;

    s = new wxZipFSInputStream(leftFile);
    if (s && s->IsOk())
    {
#if wxUSE_DATETIME
       wxDateTime dtMod;
#endif // wxUSE_DATETIME

       bool found = false;
       while (!found)
       {
           wxZipEntry *ent = s->GetNextEntry();
           if (!ent)
               break;

           if (ent->GetInternalName() == right)
           {
               found = true;
               dtMod = ent->GetDateTime();
           }

           delete ent;
       }
       if (found)
       {
           return new wxFSFile(s,
                            left + wxT("#zip:") + right,
                            GetMimeTypeFromExt(location),
                            GetAnchor(location)
#if wxUSE_DATETIME
                            , dtMod
#endif // wxUSE_DATETIME
                            );
       }
    }

    delete s;
    return NULL;
}



wxString wxZipFSHandler::FindFirst(const wxString& spec, int flags)
{
    wxString right = GetRightLocation(spec);
    wxString left = GetLeftLocation(spec);

    if (!right.empty() && right.Last() == wxT('/')) right.RemoveLast();

    if (m_Archive)
    {
        delete m_Archive;
        m_Archive = NULL;
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

    wxFSFile *leftFile = wxFileSystem().OpenFile(left);
    if (leftFile)
        m_Archive = new wxZipFSInputStream(leftFile);

    m_Pattern = right.AfterLast(wxT('/'));
    m_BaseDir = right.BeforeLast(wxT('/'));
    if (m_BaseDir.StartsWith(wxT("/")))
        m_BaseDir = m_BaseDir.Mid(1);

    if (m_Archive)
    {
        if (m_AllowDirs)
        {
            delete m_DirsFound;
            m_DirsFound = new wxZipFilenameHashMap();
            if (right.empty())  // allow "/" to match the archive root
                return spec;
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
            delete m_Archive;
            m_Archive = NULL;
            break;
        }
        namestr = entry->GetName(wxPATH_UNIX);
        delete entry;

        if (m_AllowDirs)
        {
            dir = namestr.BeforeLast(wxT('/'));
            while (!dir.empty())
            {
                if( m_DirsFound->find(dir) == m_DirsFound->end() )
                {
                    (*m_DirsFound)[dir] = 1;
                    filename = dir.AfterLast(wxT('/'));
                    dir = dir.BeforeLast(wxT('/'));
                    if (!filename.empty() && m_BaseDir == dir &&
                                wxMatchWild(m_Pattern, filename, false))
                        match = m_ZipFile + wxT("#zip:") + dir + wxT("/") + filename;
                }
                else
                    break; // already tranversed
            }
        }

        filename = namestr.AfterLast(wxT('/'));
        dir = namestr.BeforeLast(wxT('/'));
        if (m_AllowFiles && !filename.empty() && m_BaseDir == dir &&
                            wxMatchWild(m_Pattern, filename, false))
            match = m_ZipFile + wxT("#zip:") + namestr;
    }

    return match;
}



#endif
      //wxUSE_FILESYSTEM && wxUSE_FS_ZIP && wxUSE_ZIPSTREAM
