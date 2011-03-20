///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/panel.h
// Purpose:     wxUniversal-specific wxPanel class.
// Author:      Vadim Zeitlin
// Created:     2011-03-18
// RCS-ID:      $Id: wxhead.h,v 1.12 2010-04-22 12:44:51 zeitlin Exp $
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_PANEL_H_
#define _WX_UNIV_PANEL_H_

// ----------------------------------------------------------------------------
// wxPanel
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPanel : public wxPanelBase
{
public:
    wxPanel() { }

    wxPanel(wxWindow *parent,
            wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = wxPanelNameStr)
    {
        Create(parent, winid, pos, size, style, name);
    }

    virtual bool IsCanvasWindow() const { return true; }

#ifdef WXWIN_COMPATIBILITY_2_8
    wxDEPRECATED_CONSTRUCTOR(
    wxPanel(wxWindow *parent,
            int x, int y, int width, int height,
            long style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = wxPanelNameStr)
    {
        Create(parent, wxID_ANY, wxPoint(x, y), wxSize(width, height), style, name);
    }
    )
#endif // WXWIN_COMPATIBILITY_2_8

protected:
    virtual void DoSetBackgroundBitmap(const wxBitmap& bmp)
    {
        // We have support for background bitmap even at the base class level.
        SetBackground(bmp, wxALIGN_NOT, wxTILE);
    }

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxPanel);
};

#endif // _WX_UNIV_PANEL_H_
