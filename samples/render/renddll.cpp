///////////////////////////////////////////////////////////////////////////////
// Name:        renddll.cpp
// Purpose:     Example of a renderer implemented in a DLL
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.08.03
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

// derive from wxDelegateRendererNative and not wxRendererNative itself to be
// able to only reimplement the methods we want to show and not all of them
class MyDllRenderer : public wxDelegateRendererNative
{
public:
    // draw the header control button (used by wxListCtrl)
    virtual int DrawHeaderButton(wxWindow * WXUNUSED(win),
                                 wxDC& dc,
                                 const wxRect& rect,
                                 int WXUNUSED(flags) = 0,
                                 wxHeaderSortIconType WXUNUSED(sortArrow) = wxHDR_SORT_ICON_NONE,
                                 wxHeaderButtonParams* WXUNUSED(params) = NULL) wxOVERRIDE
    {
        dc.SetBrush(*wxCYAN_BRUSH);
        dc.SetTextForeground(*wxRED);
        dc.DrawRoundedRectangle(rect, 10);
        dc.DrawLabel("MyDllRenderer", wxNullBitmap, rect, wxALIGN_CENTER);

        return dc.GetTextExtent("MyDllRenderer").x;
    }

    virtual wxRendererVersion GetVersion() const wxOVERRIDE
    {
        return wxRendererVersion(wxRendererVersion::Current_Version,
                                 wxRendererVersion::Current_Age);
    }

#if 0 // just for debugging
    MyDllRenderer()
    {
        wxMessageBox("Creating MyDllRenderer", "Renderer Sample");
    }

    virtual ~MyDllRenderer()
    {
        wxMessageBox("Deleting MyDllRenderer", "Renderer Sample");
    }
#endif // 0
};

extern "C"
WXEXPORT wxRendererNative *wxCreateRenderer()
{
    return new MyDllRenderer;
}
