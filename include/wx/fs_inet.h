/////////////////////////////////////////////////////////////////////////////
// Name:        fs_inet.h
// Purpose:     HTTP and FTP file system
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FS_INET_H_
#define _WX_FS_INET_H_

#include "wx/defs.h"

#if wxUSE_FILESYSTEM && wxUSE_FS_INET && wxUSE_STREAMS && wxUSE_SOCKETS

#ifndef WXPRECOMP
    #include "wx/hash.h"
#endif

#include "wx/filesys.h"

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
  // wxUSE_FILESYSTEM && wxUSE_FS_INET && wxUSE_STREAMS && wxUSE_SOCKETS

#endif // _WX_FS_INET_H_

