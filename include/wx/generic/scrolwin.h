/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/scrolwin.h
// Purpose:     wxGenericScrolledWindow class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_SCROLLWIN_H_
#define _WX_GENERIC_SCROLLWIN_H_

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "genscrolwin.h"
#endif

// ----------------------------------------------------------------------------
// headers and constants
// ----------------------------------------------------------------------------

#include "wx/window.h"
#include "wx/panel.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxPanelNameStr;

// default scrolled window style
#ifndef wxScrolledWindowStyle
    #define wxScrolledWindowStyle (wxHSCROLL | wxVSCROLL)
#endif

// avoid triggering this stupid VC++ warning
#ifdef __VISUALC__
    #pragma warning(disable:4355) // 'this' used in base member initializer list
#endif

// ----------------------------------------------------------------------------
// wxGenericScrolledWindow
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGenericScrolledWindow : public wxPanel,
                                            public wxScrollHelper
{
public:
    wxGenericScrolledWindow() : wxScrollHelper(this) { }
    wxGenericScrolledWindow(wxWindow *parent,
                     wxWindowID id = -1,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxScrolledWindowStyle,
                     const wxString& name = wxPanelNameStr)
        : wxScrollHelper(this)
    {
        Create(parent, id, pos, size, style, name);
    }

    virtual ~wxGenericScrolledWindow();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxScrolledWindowStyle,
                const wxString& name = wxPanelNameStr);

    virtual void PrepareDC(wxDC& dc) { DoPrepareDC(dc); }

    // lay out the window and its children
    virtual bool Layout();

    virtual void DoSetVirtualSize(int x, int y);

#if WXWIN_COMPATIBILITY   
     virtual void GetScrollUnitsPerPage(int *x_page, int *y_page) const;   
     virtual void CalcUnscrolledPosition(int x, int y, float *xx, float *yy) const;
     // Need to do this otherwise the compiler gets confuced 
     // between float and int calls to this function.
     void CalcUnscrolledPosition(int x, int y, int *xx, int *yy) const
        { wxScrollHelper::CalcScrolledPosition(x, y, xx, yy); }
     wxPoint CalcUnscrolledPosition(const wxPoint& pt) const
        { return wxScrollHelper::CalcScrolledPosition(pt); }
#endif // WXWIN_COMPATIBILITY

protected:
    // this is needed for wxEVT_PAINT processing hack described in
    // wxScrollHelperEvtHandler::ProcessEvent()
    void OnPaint(wxPaintEvent& event);

    // we need to return a special WM_GETDLGCODE value to process just the
    // arrows but let the other navigation characters through
#ifdef __WXMSW__
    virtual long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
#endif // __WXMSW__

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxGenericScrolledWindow)
    DECLARE_EVENT_TABLE()
};

#ifdef __VISUALC__
    #pragma warning(default:4355)
#endif

#endif
    // _WX_GENERIC_SCROLLWIN_H_

