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
#if wxUSE_HTML && wxUSE_STREAMS

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
    #include "wx/log.h"
    #include "wx/intl.h"
#endif

#include "wx/html/htmlfilt.h"
#include "wx/html/htmlwin.h"

// utility function: read a wxString from a wxInputStream
void wxPrivate_ReadString(wxString& str, wxInputStream* s)
{
    size_t streamSize = s->GetSize();

    if(streamSize == ~(size_t)0)
    {
        const size_t bufSize = 4095;
        char buffer[bufSize+1];
        size_t lastRead;

        do
        {
            s->Read(buffer, bufSize);
            lastRead = s->LastRead();
            buffer[lastRead] = 0;
            str.Append(buffer);
        }
        while(lastRead == bufSize);
    }
    else
    {
        char* src = new char[streamSize+1];
        s->Read(src, streamSize);
        src[streamSize] = 0;
        str = src;
        delete [] src;
    }
}

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
    wxString doc, doc2;

    if (s == NULL) return wxEmptyString;
    wxPrivate_ReadString(doc, s);

    doc.Replace(wxT("&"), wxT("&amp;"), TRUE);
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
  return (file.GetMimeType().Find(wxT("text/html")) == 0);
}



wxString wxHtmlFilterHTML::ReadFile(const wxFSFile& file) const
{
    wxInputStream *s = file.GetStream();
    wxString doc;

    if (s == NULL)
    {
        wxLogError(_("Cannot open HTML document: %s"), file.GetLocation().c_str());
        return wxEmptyString;
    }
    wxPrivate_ReadString(doc, s);

    // add meta tag if we obtained this through http:
    if (file.GetMimeType().Find(_T("; charset=")) == 0)
    {
        wxString s(_T("<meta http-equiv=\"Content-Type\" content=\""));
        s << file.GetMimeType() <<  _T("\">");
        return s+doc;
    }

    return doc;
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
