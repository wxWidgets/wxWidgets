/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/scrolwin.h
// Purpose:     wxScrolledWindow class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_SCROLLWIN_H_
#define _WX_GENERIC_SCROLLWIN_H_

#ifdef __GNUG__
    #pragma interface "scrolwin.h"
#endif

// ----------------------------------------------------------------------------
// headers and constants
// ----------------------------------------------------------------------------

#include "wx/window.h"
#include "wx/panel.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxPanelNameStr;

// default scrolled window style
#define wxScrolledWindowStyle (wxHSCROLL | wxVSCROLL)

// avoid triggering this stupid VC++ warning
#ifdef __VISUALC__
    #pragma warning(disable:4355) // 'this' used in base member initializer list
#endif

// ----------------------------------------------------------------------------
// wxScrolledWindow
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxScrolledWindow : public wxPanel, public wxScrollHelper
{
public:
    wxScrolledWindow() : wxScrollHelper(this) { }
    wxScrolledWindow(wxWindow *parent,
                     wxWindowID id = -1,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxScrolledWindowStyle,
                     const wxString& name = wxPanelNameStr)
        : wxScrollHelper(this)
    {
        Create(parent, id, pos, size, style, name);
    }

    virtual ~wxScrolledWindow();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxScrolledWindowStyle,
                const wxString& name = wxPanelNameStr);

    virtual void PrepareDC(wxDC& dc) { DoPrepareDC(dc); }

private:
    DECLARE_ABSTRACT_CLASS(wxScrolledWindow)
};

#ifdef __VISUALC__
    #pragma warning(default:4355)
#endif

#endif
    // _WX_GENERIC_SCROLLWIN_H_
