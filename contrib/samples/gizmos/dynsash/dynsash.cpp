/////////////////////////////////////////////////////////////////////////////
// Name:        dynsash.cpp
// Purpose:     Test the wxDynamicSash class by creating a dynamic sash which
//              contains an HTML view
// Author:      Matt Kimball
// Modified by:
// Created:     7/15/2001
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Matt Kimball
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/app.h"
#include "wx/frame.h"
#include "wx/gizmos/dynamicsash.h"
#include "wx/html/htmlwin.h"
#include "wx/image.h"
#include "wx/cmdline.h"

class Demo : public wxApp
{
public:
    bool OnInit();
};

class SashHtmlWindow : public wxHtmlWindow
{
public:
    SashHtmlWindow(wxWindow *parent, wxWindowID id = wxID_ANY,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxHW_SCROLLBAR_NEVER,
                   const wxString& name = wxT("sashHtmlWindow"));

    wxSize DoGetBestSize() const;

private:
    void OnSplit(wxDynamicSashSplitEvent& event);

    wxWindow *m_dyn_sash;
};

IMPLEMENT_APP(Demo)

const wxChar *HTML_content =
wxT("<P><H1>wxDynamicSashWindow demo</H1>")
wxT("<P>Here is an example of how you can use <TT>wxDynamicSashWindow</TT> to allow your users to ")
wxT("dynamically split and unify the views of your windows.  Try dragging out a few splits ")
wxT("and then reunifying the window.")
wxT("<P>Also, see the <TT>dynsash_switch</TT> sample for an example of an application which ")
wxT("manages the scrollbars provided by <TT>wxDynamicSashWindow</TT> itself.");

bool Demo::OnInit()
{
    wxInitAllImageHandlers();

    wxFrame *frame = new wxFrame(NULL, wxID_ANY, wxT("Dynamic Sash Demo"));
    frame->SetSize(480, 480);

    wxDynamicSashWindow *sash = new wxDynamicSashWindow(frame, wxID_ANY);
    wxHtmlWindow *html = new SashHtmlWindow(sash, wxID_ANY);
    html->SetPage(HTML_content);

    frame->Show();

    return true;
}


SashHtmlWindow::SashHtmlWindow(wxWindow *parent, wxWindowID id,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxString& name)
              : wxHtmlWindow(parent, id, pos, size, style, name)
{
    Connect(wxEVT_DYNAMIC_SASH_SPLIT,
            wxDynamicSashSplitEventHandler(SashHtmlWindow::OnSplit));

    m_dyn_sash = parent;
}

wxSize SashHtmlWindow::DoGetBestSize() const
{
    wxHtmlContainerCell *cell = GetInternalRepresentation();
    wxSize size = GetSize();

    if (cell)
    {
        cell->Layout(size.GetWidth());
        return wxSize(cell->GetWidth(), cell->GetHeight());
    }

    return wxHtmlWindow::GetBestSize();
}

void SashHtmlWindow::OnSplit(wxDynamicSashSplitEvent& WXUNUSED(event))
{
    wxHtmlWindow *html = new SashHtmlWindow(m_dyn_sash, wxID_ANY);
    html->SetPage(HTML_content);
}
