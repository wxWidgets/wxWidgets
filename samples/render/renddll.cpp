///////////////////////////////////////////////////////////////////////////////
// Name:        renddll.cpp
// Purpose:     Example of a renderer implemented in a DLL
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.08.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/renderer.h"

#ifndef WX_PRECOMP
    #include "wx/dc.h"
#endif

class MyDllRenderer : public wxRendererNative
{
public:
    // draw the header control button (used by wxListCtrl)
    virtual void DrawHeaderButton(wxWindow *win,
                                  wxDC& dc,
                                  const wxRect& rect,
                                  int flags = 0)
    {
        dc.SetBrush(*wxCYAN_BRUSH);
        dc.SetTextForeground(*wxRED);
        dc.DrawRoundedRectangle(rect, 10);
        dc.DrawLabel(_T("MyDllRenderer"), wxNullBitmap, rect, wxALIGN_CENTER);
    }

    // draw the expanded/collapsed icon for a tree control item
    virtual void DrawTreeItemButton(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0)
    {
    }

    // draw the border for sash window: this border must be such that the sash
    // drawn by DrawSash() blends into it well
    virtual void DrawSplitterBorder(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0)
    {
    }

    // draw a (vertical) sash
    virtual void DrawSplitterSash(wxWindow *win,
                                  wxDC& dc,
                                  const wxSize& size,
                                  wxCoord position,
                                  wxOrientation orient,
                                  int flags = 0)
    {
    }

    // get the splitter parameters: the x field of the returned point is the
    // sash width and the y field is the border width
    virtual wxSplitterRenderParams GetSplitterParams(const wxWindow *win)
    {
        return wxSplitterRenderParams(0, 0, 0);
    }

    virtual wxRendererVersion GetVersion() const
    {
        return wxRendererVersion(wxRendererVersion::Current_Version,
                                 wxRendererVersion::Current_Age);
    }

#if 0 // just for debugging
    MyDllRenderer()
    {
        wxMessageBox(_T("Creating MyDllRenderer"), _T("Renderer Sample"));
    }

    virtual ~MyDllRenderer()
    {
        wxMessageBox(_T("Deleting MyDllRenderer"), _T("Renderer Sample"));
    }
#endif // 0
};

extern "C"
WXEXPORT wxRendererNative *wxCreateRenderer()
{
    return new MyDllRenderer;
}

