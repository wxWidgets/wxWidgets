/////////////////////////////////////////////////////////////////////////////
// Name:        fs_inet.h
// Purpose:     HTTP and FTP file system
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

/*

REMARKS : 

This FS creates local cache (in /tmp directory). The cache is freed
on program exit.

Size of cache is limited to cca 1000 items (due to GetTempFileName
limitation)


*/

#ifdef __GNUG__
#pragma implementation
#endif

#include <wx/wxprec.h>

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#if wxUSE_FS_INET && wxUSE_STREAMS && wxUSE_SOCKETS

#ifndef WXPRECOMP
#include <wx/wx.h>
#endif

#include <wx/filesys.h>



//--------------------------------------------------------------------------------
// wxInternetFSHandler
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxInternetFSHandler : public wxFileSystemHandler
{
    private:
        wxHashTable m_Cache;

    public:
        virtual bool CanOpen(const wxString& location);
        virtual wxFSFile* OpenFile(wxFileSystem& fs, const wxString& location);
        ~wxInternetFSHandler();
};

#endif
  // wxUSE_FS_INET && wxUSE_STREAMS && wxUSE_SOCKETS

