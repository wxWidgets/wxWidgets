/////////////////////////////////////////////////////////////////////////////
// Name:        filter.cpp
// Purpose:     wxHtmlFilter - input filter for translating into HTML format
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation
#endif

#include <wx/wxprec.h>

#include "wx/defs.h"
#if wxUSE_HTML

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include <wx/wx.h>
#endif

#include <wx/html/htmlfilter.h>
#include <wx/html/htmlwin.h>


/*

There is code for several default filters:

*/

IMPLEMENT_ABSTRACT_CLASS(wxHtmlFilter, wxObject)

//--------------------------------------------------------------------------------
// wxHtmlFilterPlainText
//          filter for text/plain or uknown
//--------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxHtmlFilterPlainText, wxHtmlFilter)

bool wxHtmlFilterPlainText::CanRead(const wxFSFile& file)
{
    return TRUE;
}



wxString wxHtmlFilterPlainText::ReadFile(const wxFSFile& file)
{
    wxInputStream *s = file.GetStream();
    char *src;
    wxString doc, doc2;

    if (s == NULL) return wxEmptyString;
    src = new char[s -> StreamSize()+1];
    src[s -> StreamSize()] = 0;
    s -> Read(src, s -> StreamSize());
    doc = src;
    delete [] src;

    doc.Replace("<", "&lt;", TRUE);
    doc.Replace(">", "&gt;", TRUE);
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
        virtual bool CanRead(const wxFSFile& file);
        virtual wxString ReadFile(const wxFSFile& file);
};

IMPLEMENT_DYNAMIC_CLASS(wxHtmlFilterImage, wxHtmlFilter)



bool wxHtmlFilterImage::CanRead(const wxFSFile& file)
{
    return (file.GetMimeType().Left(6) == "image/");
}



wxString wxHtmlFilterImage::ReadFile(const wxFSFile& file)
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
        virtual bool CanRead(const wxFSFile& file);
        virtual wxString ReadFile(const wxFSFile& file);
};


IMPLEMENT_DYNAMIC_CLASS(wxHtmlFilterHTML, wxHtmlFilter)

bool wxHtmlFilterHTML::CanRead(const wxFSFile& file)
{
    return (file.GetMimeType() == "text/html");
}



wxString wxHtmlFilterHTML::ReadFile(const wxFSFile& file)
{
    wxInputStream *s = file.GetStream();
    char *src;
    wxString doc;

    if (s == NULL) return wxEmptyString;
    src = (char*) malloc(s -> StreamSize() + 1);
    src[s -> StreamSize()] = 0;
    s -> Read(src, s -> StreamSize());
    doc = src;
    free(src);

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
