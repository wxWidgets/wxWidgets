/////////////////////////////////////////////////////////////////////////////
// Name:        wx/fs_inet.h
// Purpose:     HTTP and FTP file system
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxInternetFSHandler

    A file system handler for accessing files from internet servers.
*/
class wxInternetFSHandler : public wxFileSystemHandler
{
public:
    wxInternetFSHandler();

    /**
        Returns @true if the handler is able to open this file. This function doesn't
        check whether the file exists or not, it only checks if it knows the protocol.
    */
    virtual bool CanOpen(const wxString& location);

    /**
        Opens the file and returns wxFSFile pointer or @NULL if failed.

        @param fs
            Parent FS (the FS from that OpenFile was called).
            See the ZIP handler for details of how to use it.
        @param location
            The absolute location of file.
    */
    virtual wxFSFile* OpenFile(wxFileSystem& fs, const wxString& location);
};

