/////////////////////////////////////////////////////////////////////////////
// Name:        htmlfilt.cpp
// Purpose:     wxHtmlFilter - input filter for translating into HTML format
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation
#endif

#include "wx/wxprec.h"

#include "wx/defs.h"
#if wxUSE_HTML

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include "wx/wx.h"
#endif

#include "wx/html/htmlfilt.h"
#include "wx/html/htmlwin.h"


/*

There is code for several default filters:

*/

IMPLEMENT_ABSTRACT_CLASS(wxHtmlFilter, wxObject)

//--------------------------------------------------------------------------------
// wxHtmlFilterPlainText
//          filter for text/plain or uknown
//--------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxHtmlFilterPlainText, wxHtmlFilter)

bool wxHtmlFilterPlainText::CanRead(const wxFSFile& WXUNUSED(file)) const
{
    return TRUE;
}



wxString wxHtmlFilterPlainText::ReadFile(const wxFSFile& file) const
{
    wxInputStream *s = file.GetStream();
    char *src;
    wxString doc, doc2;

    if (s == NULL) return wxEmptyString;
    src = new char[s -> GetSize()+1];
    src[s -> GetSize()] = 0;
    s -> Read(src, s -> GetSize());
    doc = src;
    delete [] src;

    doc.Replace(wxT("<"), wxT("&lt;"), TRUE);
    doc.Replace(wxT(">"), wxT("&gt;"), TRUE);
    doc2 = "<HTML><BODY><PRE>\n" + doc + "\n</PRE></BODY></HTML>";
    return doc2;
}





//--------------------------------------------------------------------------------
// wxHtmlFilterImage
//          filter for image/*
//--------------------------------------------------------------------------------

class wxHtmlFilterImage : public wxHtmlFilter
{
    DECLARE_DYNAMIC_CLASS(wxHtmlFilterImage)

    public:
        virtual bool CanRead(const wxFSFile& file) const;
        virtual wxString ReadFile(const wxFSFile& file) const;
};

IMPLEMENT_DYNAMIC_CLASS(wxHtmlFilterImage, wxHtmlFilter)



bool wxHtmlFilterImage::CanRead(const wxFSFile& file) const
{
    return (file.GetMimeType().Left(6) == wxT("image/"));
}



wxString wxHtmlFilterImage::ReadFile(const wxFSFile& file) const
{
    return ("<HTML><BODY><IMG SRC=\"" + file.GetLocation() + "\"></BODY></HTML>");
}




//--------------------------------------------------------------------------------
// wxHtmlFilterPlainText
//          filter for text/plain or uknown
//--------------------------------------------------------------------------------

class wxHtmlFilterHTML : public wxHtmlFilter
{
    DECLARE_DYNAMIC_CLASS(wxHtmlFilterHTML)

    public:
        virtual bool CanRead(const wxFSFile& file) const;
        virtual wxString ReadFile(const wxFSFile& file) const;
};


IMPLEMENT_DYNAMIC_CLASS(wxHtmlFilterHTML, wxHtmlFilter)

bool wxHtmlFilterHTML::CanRead(const wxFSFile& file) const
{
//    return (file.GetMimeType() == "text/html");
// This is true in most case but some page can return:
// "text/html; char-encoding=...."
// So we use Find instead
  return (file.GetMimeType().Find(_("text/html")) == 0);
}



wxString wxHtmlFilterHTML::ReadFile(const wxFSFile& file) const
{
    wxInputStream *s = file.GetStream();
    char *src;
    wxString doc;

    if (s == NULL) 
    {
        wxLogError(_("Cannot open HTML document: %s"), file.GetLocation().mb_str());
        return wxEmptyString;
    }
    src = new char[s -> GetSize() + 1];
    src[s -> GetSize()] = 0;
    s -> Read(src, s -> GetSize());
    doc = src;
    delete[] src;

    // add meta tag if we obtained this through http:    
    if (file.GetMimeType().Find(_T("; charset=")) == 0)
    {
        wxString s(_T("<meta http-equiv=\"Content-Type\" content=\""));
        s << file.GetMimeType() <<  _T("\">");
        return s;
    }

    else return doc;
}




///// Module:

class wxHtmlFilterModule : public wxModule
{
    DECLARE_DYNAMIC_CLASS(wxHtmlFilterModule)

    public:
        virtual bool OnInit()
        {
            wxHtmlWindow::AddFilter(new wxHtmlFilterHTML);
            wxHtmlWindow::AddFilter(new wxHtmlFilterImage);
            return TRUE;
        }
        virtual void OnExit() {}
};

IMPLEMENT_DYNAMIC_CLASS(wxHtmlFilterModule, wxModule)

#endif
