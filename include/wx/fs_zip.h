/////////////////////////////////////////////////////////////////////////////
// Name:        fs_zip.h
// Purpose:     ZIP file system
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation
#endif

#include <wx/wxprec.h>

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#if wxUSE_FS_ZIP && wxUSE_STREAMS


#ifndef WXPRECOMP
#include <wx/wx.h>
#endif

#include <wx/filesys.h>



//--------------------------------------------------------------------------------
// wxZipFSHandler
//--------------------------------------------------------------------------------

class wxZipFSHandler : public wxFileSystemHandler
{
    public:
        virtual bool CanOpen(const wxString& location);
        virtual wxFSFile* OpenFile(wxFileSystem& fs, const wxString& location);
        ~wxZipFSHandler();
};


#endif
  // wxUSE_FS_ZIP && wxUSE_STREAMS

