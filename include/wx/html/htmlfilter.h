/////////////////////////////////////////////////////////////////////////////
// Name:        htmlfilter.h
// Purpose:     filters
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __HTMLFILTER_H__
#define __HTMLFILTER_H__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#if wxUSE_HTML

#include <wx/filesys.h>


//--------------------------------------------------------------------------------
// wxHtmlFilter
//                  This class is input filter. It can "translate" files
//                  in non-HTML format to HTML format
//                  interface to access certain
//                  kinds of files (HTPP, FTP, local, tar.gz etc..)
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxHtmlFilter : public wxObject
{
    DECLARE_ABSTRACT_CLASS(wxHtmlFilter)

    public:
        wxHtmlFilter() : wxObject() {}

        virtual bool CanRead(const wxFSFile& file) = 0;
                // returns TRUE if this filter is able to open&read given file

        virtual wxString ReadFile(const wxFSFile& file) = 0;
                // reads given file and returns HTML document.
                // Returns empty string if opening failed
};



//--------------------------------------------------------------------------------
// wxHtmlFilterPlainText
//                  This filter is used as default filter if no other can
//                  be used (= uknown type of file). It is used by
//                  wxHtmlWindow itself.
//--------------------------------------------------------------------------------


class WXDLLEXPORT wxHtmlFilterPlainText : public wxHtmlFilter
{
    DECLARE_DYNAMIC_CLASS(wxHtmlFilterPlainText)

    public:
        virtual bool CanRead(const wxFSFile& file);
        virtual wxString ReadFile(const wxFSFile& file);
};




#endif // __HTMLFILTER_H__

#endif