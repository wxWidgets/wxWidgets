/////////////////////////////////////////////////////////////////////////////
// Name:        webfilehandler.h
// Purpose:     interface of wxWebFileHandler
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxWebFileHandler
  
    A custom handler for the file scheme which also supports loading from 
    archives. The syntax for wxWebFileHandler differs from virtual file 
    systems in the rest of wxWidgets by using a syntax such as
    @c file:///C:/exmaple/docs.zip;protocol=zip/main.htm Currently the only
    supported protocol is @c zip. 
   
    @library{wxweb}
    @category{web}
    
    @see wxWebView, wxWebHandler
 */
class wxWebFileHandler : public wxWebHandler
{
public:
    /**
        @return The string @c "file"
    */
    virtual wxString GetName() const;
    virtual wxFSFile* GetFile(const wxString &uri);
};