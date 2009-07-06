///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/panel.h
// Purpose:     Ribbon-style container for a group of related tools / controls
// Author:      Peter Cawley
// Modified by:
// Created:     2009-05-25
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////
#ifndef _WX_RIBBON_PANEL_H_
#define _WX_RIBBON_PANEL_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

#include "wx/bitmap.h"
#include "wx/ribbon/control.h"

enum wxRibbonPanelOption
{
    wxRIBBON_PANEL_NO_AUTO_MINIMISE    = 1 << 0,
    wxRIBBON_PANEL_EXT_BUTTON        = 1 << 3,
    wxRIBBON_PANEL_MINIMISE_BUTTON    = 1 << 4,
    
    wxRIBBON_PANEL_DEFAULT_STYLE = 0,
};

class WXDLLIMPEXP_RIBBON wxRibbonPanel : public wxRibbonControl
{
public:
    wxRibbonPanel();

    wxRibbonPanel(wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxString& label = wxEmptyString,
                  const wxBitmap& minimised_icon = wxNullBitmap,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxRIBBON_PANEL_DEFAULT_STYLE);

    virtual ~wxRibbonPanel();

    bool Create(wxWindow* parent,
                wxWindowID id = wxID_ANY,
                const wxString& label = wxEmptyString,
                const wxBitmap& icon = wxNullBitmap,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxRIBBON_PANEL_DEFAULT_STYLE);

    wxBitmap& GetMinimisedIcon() {return m_minimised_icon;}

    void SetArtProvider(wxRibbonArtProvider* art);

    virtual bool Realize();
    virtual bool Layout();
    virtual wxSize GetMinSize() const;
    virtual wxSize DoGetBestSize() const;

    virtual bool IsSizingContinuous() const;
    virtual wxSize GetNextSmallerSize(wxOrientation direction) const;
    virtual wxSize GetNextLargerSize(wxOrientation direction) const;

protected:
    wxBorder GetDefaultBorder() const { return wxBORDER_NONE; }

    void OnSize(wxSizeEvent& evt);
    void OnEraseBackground(wxEraseEvent& evt);
    void OnPaint(wxPaintEvent& evt);

    void CommonInit(const wxString& label, const wxBitmap& icon, long style);

    wxBitmap m_minimised_icon;
    long m_flags;

#ifndef SWIG
    DECLARE_CLASS(wxRibbonPanel)
    DECLARE_EVENT_TABLE()
#endif
};

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_PANEL_H_
