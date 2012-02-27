/////////////////////////////////////////////////////////////////////////////
// Name:        wx/fs_arc.h
// Purpose:     Archive file system
// Author:      Vaclav Slavik, Mike Wetherell
// Copyright:   (c) 1999 Vaclav Slavik, (c) 2006 Mike Wetherell
// CVS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


class wxArchiveFSHandler : public wxFileSystemHandler
{
public:
    wxArchiveFSHandler();
    virtual ~wxArchiveFSHandler();
    void Cleanup();
};


typedef wxArchiveFSHandler wxZipFSHandler;
