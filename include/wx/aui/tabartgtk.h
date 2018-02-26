/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/aui/tabartgtk.h
// Purpose:     declaration of the wxAuiGTKTabArt
// Author:      Jens Lody and Teodor Petrov
// Modified by: Malcolm MacLeod (mmacleod@webmail.co.za)
// Modified by: Jens Lody
// Created:     2012-03-23
// Copyright:   (c) 2012 Jens Lody <jens@codeblocks.org>
//                  and Teodor Petrov
//              2012, Jens Lody for the code related to left and right
//                  positioning
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_AUI_TABARTGTK_H_
#define _WX_AUI_TABARTGTK_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"

#if wxUSE_AUI

#include "wx/aui/tabart.h"
#include "wx/gdicmn.h"
#include "wx/aui/auibook.h"

class wxWindow;
class wxDC;

class WXDLLIMPEXP_AUI wxAuiGtkTabArt : public wxAuiGenericTabArt
{
public:
    wxAuiGtkTabArt();

    virtual wxAuiTabArt* Clone();
    virtual void DrawBorder(wxDC& dc, wxWindow* wnd, const wxRect& rect);
    virtual void DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect);
    virtual void DrawTab(wxDC& dc,
                         wxWindow* wnd,
                         const wxAuiPaneInfo& page,
                         const wxRect& inRect,
                         int close_buttonState,
                         bool haveFocus,
                         wxRect* outTabRect,
                         wxRect* outButtonRect,
                         int* xExtent);
    void DrawButton(wxDC& dc, wxWindow* wnd, const wxRect& inRect, int bitmapID, int buttonState, int orientation, wxRect* outRect);
    wxSize GetBestTabSize(wxWindow* wnd, const wxAuiPaneInfoPtrArray& pages, const wxSize& requiredBmpSize);
    int GetBestTabCtrlSize(wxWindow* wnd, const wxAuiPaneInfoPtrArray& pages)
                         {
                             return GetBestTabSize(wnd, pages, m_requiredBitmapSize).GetHeight();
                         }
    int GetBorderWidth(wxWindow* wnd);
    int GetAdditionalBorderSpace(wxWindow* wnd);
    virtual wxSize GetTabSize(wxDC& dc, wxWindow* wnd, const wxString& caption,
                              const wxBitmap& bitmap, bool active,
                              int closeButtonState, int* extent);
};

#endif  // wxUSE_AUI

#endif // _WX_AUI_TABARTGTK_H_
