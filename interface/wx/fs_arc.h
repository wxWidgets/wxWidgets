/////////////////////////////////////////////////////////////////////////////
// Name:        wx/fs_arc.h
// Purpose:     Archive file system
// Author:      Vaclav Slavik, Mike Wetherell
// Copyright:   (c) 1999 Vaclav Slavik, (c) 2006 Mike Wetherell
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxArchiveFSHandler

    A file system handler for accessing files inside of archives.
*/
class wxArchiveFSHandler : public wxFileSystemHandler
{
public:
    wxArchiveFSHandler();

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

    /**
        Works like ::wxFindFirstFile().

        Returns the name of the first filename (within filesystem's current path)
        that matches @e wildcard. @a flags may be one of wxFILE (only files),
        wxDIR (only directories) or 0 (both).

        This method is only called if CanOpen() returns @true.
    */
    virtual wxString FindFirst(const wxString& spec, int flags = 0);

    /**
        Returns next filename that matches parameters passed to wxFileSystem::FindFirst.

        This method is only called if CanOpen() returns @true and FindFirst()
        returned a non-empty string.
    */
    virtual wxString FindNext();

    virtual ~wxArchiveFSHandler();
    void Cleanup();
};


typedef wxArchiveFSHandler wxZipFSHandler;
