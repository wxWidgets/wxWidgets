/////////////////////////////////////////////////////////////////////////////
// Name:        fs_inet.h
// Purpose:     HTTP and FTP file system
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FS_INET_H_
#define _WX_FS_INET_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "fs_inet.h"
#endif

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

