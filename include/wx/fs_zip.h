/////////////////////////////////////////////////////////////////////////////
// Name:        fs_zip.h
// Purpose:     ZIP file system
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// CVS-ID:      $Id$
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation
#endif

#include "wx/wxprec.h"

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#if wxUSE_FILESYSTEM && wxUSE_FS_ZIP && wxUSE_STREAMS


#ifndef WXPRECOMP
#include "wx/wx.h"
#endif

#include "wx/filesys.h"

class WXDLLEXPORT wxHashTableLong;

//--------------------------------------------------------------------------------
// wxZipFSHandler
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxZipFSHandler : public wxFileSystemHandler
{
    public:
        wxZipFSHandler();
        virtual bool CanOpen(const wxString& location);
        virtual wxFSFile* OpenFile(wxFileSystem& fs, const wxString& location);
        virtual wxString FindFirst(const wxString& spec, int flags = 0);
        virtual wxString FindNext();
        ~wxZipFSHandler();
        
    private:
        // these vars are used by FindFirst/Next:
        void *m_Archive;
        wxString m_Pattern, m_BaseDir, m_ZipFile;
        bool m_AllowDirs, m_AllowFiles;
        wxHashTableLong *m_DirsFound;
        
        wxString DoFind();
};


#endif
  // wxUSE_FILESYSTEM && wxUSE_FS_ZIP && wxUSE_STREAMS

