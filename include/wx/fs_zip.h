/////////////////////////////////////////////////////////////////////////////
// Name:        fs_zip.h
// Purpose:     ZIP file system
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// CVS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FS_ZIP_H_
#define _WX_FS_ZIP_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "fs_zip.h"
#endif

#include "wx/defs.h"

#if wxUSE_FILESYSTEM && wxUSE_FS_ZIP && wxUSE_STREAMS

#include "wx/filesys.h"
#include "wx/hashmap.h"

//---------------------------------------------------------------------------
// wxZipFSHandler
//---------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxZipFSHandler : public wxFileSystemHandler
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
        wxLongToLongHashMap *m_DirsFound;

        wxString DoFind();

    DECLARE_NO_COPY_CLASS(wxZipFSHandler)
};


#endif
  // wxUSE_FILESYSTEM && wxUSE_FS_ZIP && wxUSE_STREAMS

#endif // _WX_FS_ZIP_H_

