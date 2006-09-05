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

#include "wx/defs.h"

#if wxUSE_FILESYSTEM && wxUSE_FS_ZIP && wxUSE_STREAMS

#include "wx/filesys.h"
#include "wx/hashmap.h"


WX_DECLARE_STRING_HASH_MAP(int, wxZipFilenameHashMap);


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
        void Cleanup();
        virtual ~wxZipFSHandler();

    private:
        // these vars are used by FindFirst/Next:
        class wxZipInputStream *m_Archive;
        wxString m_Pattern, m_BaseDir, m_ZipFile;
        bool m_AllowDirs, m_AllowFiles;
        wxZipFilenameHashMap *m_DirsFound;

        wxString DoFind();

    DECLARE_NO_COPY_CLASS(wxZipFSHandler)
};


#endif
  // wxUSE_FILESYSTEM && wxUSE_FS_ZIP && wxUSE_STREAMS

#endif // _WX_FS_ZIP_H_

