/////////////////////////////////////////////////////////////////////////////
// Name:        webfilehandler.h
// Purpose:     interface of wxWebViewArchiveHandler
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxWebViewArchiveHandler
  
    A custom handler for the file scheme which also supports loading from 
    archives. The syntax for wxWebViewArchiveHandler differs from virtual file 
    systems in the rest of wxWidgets by using a syntax such as
    <code> scheme:///C:/exmaple/docs.zip;protocol=zip/main.htm </code> 
    Currently the only supported protocol is @c zip. 
   
    @library{wxweb}
    @category{web}
    
    @see wxWebView, wxWebViewHandler
 */
class wxWebViewArchiveHandler : public wxWebViewHandler
{
public:
    /**
        Constructor.
    */
    wxWebViewArchiveHandler(const wxString& scheme);
    virtual wxFSFile* GetFile(const wxString &uri);
};